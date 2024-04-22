#include "api.h"

#include <stdio.h>

#include <enet/enet.h>

#include "lib/wren/wren.h"

void enetClose()
{
    enet_deinitialize();
}

void enetInit(WrenVM* vm)
{
    if (enet_initialize() < 0) {
        VM_ABORT(vm, "Failed to initialize ENet.");
        return;
    }

    vmData* data = (vmData*)wrenGetUserData(vm);
    data->enetInit = true;
}

void enetGetVersion(WrenVM* vm)
{
    static char version[32];
    sprintf(version, "%d.%d.%d", ENET_VERSION_MAJOR, ENET_VERSION_MINOR, ENET_VERSION_PATCH);
    wrenEnsureSlots(vm, 1);
    wrenSetSlotString(vm, 0, version);
}

void hostAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(ENetHost*));
}

void hostFinalize(void* data)
{
    ENetHost** host = (ENetHost**)data;

    if (*host)
        enet_host_destroy(*host);

    *host = NULL;
}

// Thanks to: https://github.com/leafo/lua-enet
static void parse_address(WrenVM* vm, const char* addr_str, ENetAddress* address)
{
    int host_i = 0, port_i = 0;
    char host_str[128] = { 0 };
    char port_str[32] = { 0 };
    int scanning_port = 0;

    char* c = (char*)addr_str;

    while (*c != 0) {
        if (host_i >= 128 || port_i >= 32) {
            VM_ABORT(vm, "Hostname too long.");
            return;
        }
        if (scanning_port) {
            port_str[port_i++] = *c;
        } else {
            if (*c == ':') {
                scanning_port = 1;
            } else {
                host_str[host_i++] = *c;
            }
        }
        c++;
    }
    host_str[host_i] = '\0';
    port_str[port_i] = '\0';

    if (host_i == 0) {
        VM_ABORT(vm, "Failed to parse address.");
        return;
    }
    if (port_i == 0) {
        VM_ABORT(vm, "Missing port in address.");
        return;
    }

    if (strcmp("*", host_str) == 0) {
        address->host = ENET_HOST_ANY;
    } else {
        if (enet_address_set_host(address, host_str) != 0) {
            VM_ABORT(vm, "Failed to resolve hostname.");
            return;
        }
    }

    if (strcmp("*", port_str) == 0) {
        address->port = ENET_PORT_ANY;
    } else {
        address->port = atoi(port_str);
    }
}

void hostNew(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 2, NUM, "peerCount");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "channelCount");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "inBandwidth");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "outBandwidth");
    int peerCount = (int)wrenGetSlotDouble(vm, 2);
    int channelCount = (int)wrenGetSlotDouble(vm, 3);
    int inBandwidth = (int)wrenGetSlotDouble(vm, 4);
    int outBandwidth = (int)wrenGetSlotDouble(vm, 5);

    if (wrenGetSlotType(vm, 1) == WREN_TYPE_NULL) {
        *host = enet_host_create(NULL, peerCount, channelCount, inBandwidth, outBandwidth);
    } else if (wrenGetSlotType(vm, 1) == WREN_TYPE_STRING) {
        const char* address = wrenGetSlotString(vm, 1);
        ENetAddress addr;
        parse_address(vm, address, &addr);
        *host = enet_host_create(&addr, peerCount, channelCount, inBandwidth, outBandwidth);
    } else {
        VM_ABORT(vm, "Invalid address type.");
        return;
    }

    if (*host == NULL) {
        VM_ABORT(vm, "Failed to create ENet host.");
        return;
    }
}

void hostConnect(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "address");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "channelCount");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "data");
    const char* address = wrenGetSlotString(vm, 1);
    int channelCount = (int)wrenGetSlotDouble(vm, 2);
    int data = (int)wrenGetSlotDouble(vm, 3);

    ENetAddress addr;
    parse_address(vm, address, &addr);

    ENetPeer* peer = enet_host_connect(*host, &addr, channelCount, data);
    if (peer == NULL) {
        VM_ABORT(vm, "Failed to connect to ENet host.");
        return;
    }

    vmData* d = (vmData*)wrenGetUserData(vm);
    wrenSetSlotHandle(vm, 1, d->peerClass);
    ENetPeer** p = wrenSetSlotNewForeign(vm, 0, 1, sizeof(ENetPeer*));
    *p = peer;
}

static void pushEvent(WrenVM* vm, ENetEvent* event)
{
    wrenEnsureSlots(vm, 4);
    wrenSetSlotNewMap(vm, 0);

    if (event->peer) {
        wrenSetSlotString(vm, 1, "peer");

        vmData* data = (vmData*)wrenGetUserData(vm);
        wrenSetSlotHandle(vm, 3, data->peerClass);
        ENetPeer** p = wrenSetSlotNewForeign(vm, 2, 3, sizeof(ENetPeer*));
        *p = event->peer;

        wrenSetMapValue(vm, 0, 1, 2);
    }

    switch (event->type) {
    case ENET_EVENT_TYPE_CONNECT:
        wrenSetSlotString(vm, 1, "data");
        wrenSetSlotDouble(vm, 2, event->data);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 2, "connect");
        break;
    case ENET_EVENT_TYPE_DISCONNECT:
        wrenSetSlotString(vm, 1, "data");
        wrenSetSlotDouble(vm, 2, event->data);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 2, "disconnect");
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        wrenSetSlotString(vm, 1, "data");
        wrenSetSlotBytes(vm, 2, event->packet->data, event->packet->dataLength);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 1, "channel");
        wrenSetSlotDouble(vm, 2, event->channelID);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 2, "receive");

        enet_packet_destroy(event->packet);
        break;
    case ENET_EVENT_TYPE_NONE:
        wrenSetSlotString(vm, 2, "none");
        break;
    }

    wrenSetSlotString(vm, 1, "type");
    wrenSetMapValue(vm, 0, 1, 2);
}

void hostService(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "timeout");
    int timeout = (int)wrenGetSlotDouble(vm, 1);

    ENetEvent event;
    int status = enet_host_service(*host, &event, timeout);
    if (status == 0) {
        wrenSetSlotNull(vm, 0);
        return;
    } else if (status < 0) {
        VM_ABORT(vm, "Failed to service ENet host.");
        return;
    }

    pushEvent(vm, &event);
}

void hostCheckEvents(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);

    ENetEvent event;
    int status = enet_host_check_events(*host, &event);
    if (status == 0) {
        wrenSetSlotNull(vm, 0);
        return;
    } else if (status < 0) {
        VM_ABORT(vm, "Failed to check events on ENet host.");
        return;
    }

    pushEvent(vm, &event);
}

void hostCompressWithRangeCoder(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    int result = enet_host_compress_with_range_coder(*host);

    if (result == 0) {
        wrenSetSlotBool(vm, 0, true);
    } else {
        wrenSetSlotBool(vm, 0, false);
    }
}

void hostFlush(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    enet_host_flush(*host);
}

void hostBroadcast(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "channel");
    ASSERT_SLOT_TYPE(vm, 3, STRING, "flag");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);
    int channel = (int)wrenGetSlotDouble(vm, 2);
    const char* flagString = wrenGetSlotString(vm, 3);

    int flag = ENET_PACKET_FLAG_RELIABLE;

    if (strcmp(flagString, "unsequenced") == 0) {
        flag = ENET_PACKET_FLAG_UNSEQUENCED;
    } else if (strcmp(flagString, "reliable") == 0) {
        flag = ENET_PACKET_FLAG_RELIABLE;
    } else if (strcmp(flagString, "unreliable") == 0) {
        flag = 0;
    } else {
        VM_ABORT(vm, "Invalid packet flag.");
        return;
    }

    ENetPacket* packet = enet_packet_create(data, length, flag);
    if (packet == NULL) {
        VM_ABORT(vm, "Failed to create packet.");
        return;
    }

    enet_host_broadcast(*host, channel, packet);
}

void hostSetBandwidthLimit(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "incoming");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "outgoing");
    int incoming = (int)wrenGetSlotDouble(vm, 1);
    int outgoing = (int)wrenGetSlotDouble(vm, 2);
    enet_host_bandwidth_limit(*host, incoming, outgoing);
}

void hostGetPeer(WrenVM* vm)
{
    ENetHost* host = *(ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "index");
    int index = (int)wrenGetSlotDouble(vm, 1);

    if (index < 0 || index >= host->peerCount) {
        VM_ABORT(vm, "Invalid peer index.");
        return;
    }

    ENetPeer* peer = &(host->peers[index]);

    vmData* data = (vmData*)wrenGetUserData(vm);
    wrenSetSlotHandle(vm, 1, data->peerClass);
    ENetPeer** p = wrenSetSlotNewForeign(vm, 0, 1, sizeof(ENetPeer*));
    *p = peer;
}

void hostGetTotalSent(WrenVM* vm)
{
    ENetHost* host = *(ENetHost**)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, host->totalSentData);
}

void hostGetTotalReceived(WrenVM* vm)
{
    ENetHost* host = *(ENetHost**)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, host->totalReceivedData);
}

void hostGetServiceTime(WrenVM* vm)
{
    ENetHost* host = *(ENetHost**)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, host->serviceTime);
}

void hostGetPeerCount(WrenVM* vm)
{
    ENetHost* host = *(ENetHost**)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, host->peerCount);
}

void hostGetAddress(WrenVM* vm)
{
    ENetHost* host = *(ENetHost**)wrenGetSlotForeign(vm, 0);

    ENetAddress addr;
    enet_socket_get_address(host->socket, &addr);

    char address[128];
    sprintf(address, "%d.%d.%d.%d:%d", addr.host & 0xFF, (addr.host >> 8) & 0xFF, (addr.host >> 16) & 0xFF, (addr.host >> 24) & 0xFF, addr.port);
    wrenSetSlotString(vm, 0, address);
}

void hostSetChannelLimit(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "limit");
    int limit = (int)wrenGetSlotDouble(vm, 1);
    enet_host_channel_limit(*host, limit);
}

void peerDisconnect(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "data");
    int data = (int)wrenGetSlotDouble(vm, 1);
    enet_peer_disconnect(*peer, data);
}

void peerDisconnectNow(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "data");
    int data = (int)wrenGetSlotDouble(vm, 1);
    enet_peer_disconnect_now(*peer, data);
}

void peerDisconnectLater(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "data");
    int data = (int)wrenGetSlotDouble(vm, 1);
    enet_peer_disconnect_later(*peer, data);
}

void peerPing(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    enet_peer_ping(*peer);
}

void peerReset(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    enet_peer_reset(*peer);
}

void peerSend(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "channel");
    ASSERT_SLOT_TYPE(vm, 3, STRING, "flag");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);
    int channel = (int)wrenGetSlotDouble(vm, 2);
    const char* flagString = wrenGetSlotString(vm, 3);

    int flag = ENET_PACKET_FLAG_RELIABLE;

    if (strcmp(flagString, "unsequenced") == 0) {
        flag = ENET_PACKET_FLAG_UNSEQUENCED;
    } else if (strcmp(flagString, "reliable") == 0) {
        flag = ENET_PACKET_FLAG_RELIABLE;
    } else if (strcmp(flagString, "unreliable") == 0) {
        flag = 0;
    } else {
        VM_ABORT(vm, "Invalid packet flag.");
        return;
    }

    ENetPacket* packet = enet_packet_create(data, length, flag);
    if (packet == NULL) {
        VM_ABORT(vm, "Failed to create packet.");
        return;
    }

    enet_peer_send(*peer, channel, packet);
}

void peerReceive(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);

    enet_uint8 channel;
    ENetPacket* packet = enet_peer_receive(*peer, &channel);
    if (packet == NULL) {
        wrenSetSlotNull(vm, 0);
        return;
    }

    wrenEnsureSlots(vm, 3);
    wrenSetSlotNewMap(vm, 0);

    wrenSetSlotString(vm, 1, "data");
    wrenSetSlotBytes(vm, 2, packet->data, packet->dataLength);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "channel");
    wrenSetSlotDouble(vm, 2, channel);
    wrenSetMapValue(vm, 0, 1, 2);

    enet_packet_destroy(packet);
}

void peerConfigThrottle(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "interval");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "acceleration");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "deceleration");
    int interval = (int)wrenGetSlotDouble(vm, 1);
    int acceleration = (int)wrenGetSlotDouble(vm, 2);
    int deceleration = (int)wrenGetSlotDouble(vm, 3);
    enet_peer_throttle_configure(*peer, interval, acceleration, deceleration);
}

void peerSetTimeout(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "limit");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "minimum");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "maximum");
    int limit = (int)wrenGetSlotDouble(vm, 1);
    int minimum = (int)wrenGetSlotDouble(vm, 2);
    int maximum = (int)wrenGetSlotDouble(vm, 3);
    enet_peer_timeout(*peer, limit, minimum, maximum);
}

void peerGetConnectId(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, peer->connectID);
}

static size_t findPeerIndex(WrenVM* vm, ENetHost* host, ENetPeer* peer)
{
    size_t index;
    for (index = 0; index < host->peerCount; index++) {
        if (peer == &(host->peers[index]))
            return index;
    }

    VM_ABORT(vm, "Failed to find peer index.");
    return index;
}

void peerGetIndex(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);
    size_t index = findPeerIndex(vm, peer->host, peer);
    wrenSetSlotDouble(vm, 0, index);
}

void peerGetState(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);

    switch (peer->state) {
    case (ENET_PEER_STATE_DISCONNECTED):
        wrenSetSlotString(vm, 0, "disconnected");
        break;
    case (ENET_PEER_STATE_CONNECTING):
        wrenSetSlotString(vm, 0, "connecting");
        break;
    case (ENET_PEER_STATE_ACKNOWLEDGING_CONNECT):
        wrenSetSlotString(vm, 0, "acknowledging_connect");
        break;
    case (ENET_PEER_STATE_CONNECTION_PENDING):
        wrenSetSlotString(vm, 0, "connection_pending");
        break;
    case (ENET_PEER_STATE_CONNECTION_SUCCEEDED):
        wrenSetSlotString(vm, 0, "connection_succeeded");
        break;
    case (ENET_PEER_STATE_CONNECTED):
        wrenSetSlotString(vm, 0, "connected");
        break;
    case (ENET_PEER_STATE_DISCONNECT_LATER):
        wrenSetSlotString(vm, 0, "disconnect_later");
        break;
    case (ENET_PEER_STATE_DISCONNECTING):
        wrenSetSlotString(vm, 0, "disconnecting");
        break;
    case (ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT):
        wrenSetSlotString(vm, 0, "acknowledging_disconnect");
        break;
    case (ENET_PEER_STATE_ZOMBIE):
        wrenSetSlotString(vm, 0, "zombie");
        break;
    default:
        wrenSetSlotString(vm, 0, "unknown");
    }
}

void peerGetRtt(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, peer->roundTripTime);
}

void peerGetLastRtt(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, peer->lastRoundTripTime);
}

void peerGetTimeout(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);

    wrenEnsureSlots(vm, 2);
    wrenSetSlotNewMap(vm, 0);

    wrenSetSlotString(vm, 1, "limit");
    wrenSetSlotDouble(vm, 2, peer->timeoutLimit);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "minimum");
    wrenSetSlotDouble(vm, 2, peer->timeoutMinimum);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "maximum");
    wrenSetSlotDouble(vm, 2, peer->timeoutMaximum);
    wrenSetMapValue(vm, 0, 1, 2);
}

void peerGetToString(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);
    char host[128];
    enet_address_get_host_ip(&peer->address, host, 128);
    sprintf(host + strlen(host), ":%d", peer->address.port);
    wrenSetSlotString(vm, 0, host);
}

void peerSetPingInterval(WrenVM* vm)
{
    ENetPeer** peer = (ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "interval");
    int interval = (int)wrenGetSlotDouble(vm, 1);
    enet_peer_ping_interval(*peer, interval);
}

void peerSetRtt(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "rtt");
    int rtt = (int)wrenGetSlotDouble(vm, 1);
    peer->roundTripTime = rtt;
}

void peerSetLastRtt(WrenVM* vm)
{
    ENetPeer* peer = *(ENetPeer**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "lastRtt");
    int lastRtt = (int)wrenGetSlotDouble(vm, 1);
    peer->lastRoundTripTime = lastRtt;
}
