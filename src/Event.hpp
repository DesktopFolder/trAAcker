#pragma once

namespace aa
{
struct AdvancementStatus;
struct AdvancementManifest;

// Observer pointer... where are you... :sob:
template<typename T>
using weak_ptr = T*;

struct Event {};

// What kind of events can we have?
struct ManifestUpdate : Event
{
    const weak_ptr<AdvancementManifest> new_manifest;
};

struct StatusUpdate : Event
{
    const weak_ptr<AdvancementStatus> new_status;
};

struct EventHandler
{
    virtual void handle_event(ManifestUpdate) {}
    virtual void handle_event(StatusUpdate) {}
};
}