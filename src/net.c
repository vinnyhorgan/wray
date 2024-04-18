#include "api.h"

#include <stdio.h>

#include <enet/enet.h>

#include "lib/wren/wren.h"

void enetInit(WrenVM* vm)
{
    if (enet_initialize() < 0) {
        VM_ABORT(vm, "Failed to initialize ENet.")
        return;
    }
}

void enetClose(WrenVM* vm)
{
    enet_deinitialize();
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

    if (*host != NULL) {
        enet_host_destroy(*host);
        *host = NULL;
    }
}

void hostNew(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);

    *host = enet_host_create(NULL, 64, 1, 0, 0);
    if (*host == NULL) {
        VM_ABORT(vm, "Failed to create ENet host.")
        return;
    }
}

void hostNew2(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "port");
    int port = (int)wrenGetSlotDouble(vm, 1);

    ENetAddress addr;
    addr.host = ENET_HOST_ANY;
    addr.port = port;

    *host = enet_host_create(&addr, 64, 1, 0, 0);
    if (*host == NULL) {
        VM_ABORT(vm, "Failed to create ENet host.")
        return;
    }
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
        VM_ABORT(vm, "Failed to service ENet host.")
        return;
    }

    wrenSetSlotNewMap(vm, 0);

    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
        wrenSetSlotString(vm, 1, "data");
        wrenSetSlotDouble(vm, 2, event.data);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 2, "connect");
        break;
    case ENET_EVENT_TYPE_DISCONNECT:
        wrenSetSlotString(vm, 1, "data");
        wrenSetSlotDouble(vm, 2, event.data);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 2, "disconnect");
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        wrenSetSlotString(vm, 1, "data");
        wrenSetSlotBytes(vm, 2, event.packet->data, event.packet->dataLength);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 1, "channel");
        wrenSetSlotDouble(vm, 2, event.channelID);
        wrenSetMapValue(vm, 0, 1, 2);

        wrenSetSlotString(vm, 2, "receive");

        enet_packet_destroy(event.packet);
        break;
    case ENET_EVENT_TYPE_NONE:
        wrenSetSlotString(vm, 2, "none");
        break;
    }

    wrenSetSlotString(vm, 1, "type");
    wrenSetMapValue(vm, 0, 1, 2);
}

void hostConnect(WrenVM* vm)
{
    ENetHost** host = (ENetHost**)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "address");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "port");
    const char* address = wrenGetSlotString(vm, 1);
    int port = (int)wrenGetSlotDouble(vm, 2);

    ENetAddress addr;
    enet_address_set_host(&addr, address);
    addr.port = port;

    enet_host_connect(*host, &addr, 1, 0);
}