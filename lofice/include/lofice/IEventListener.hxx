/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI Event Hooking API
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 * Rights Holder: Lonex. Inc — shinkang888@gmail.com
 * Inventor: kangjunchul8@gmail.com
 * Registered Copyright Program — Republic of Korea
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_IEVENTLISTENER_HXX
#define INCLUDED_LOFICE_IEVENTLISTENER_HXX

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace lofice
{

/** Event categories mirroring VCL → SFX → Model pipeline (Phase 1 mapping). */
enum class EventCategory : std::uint8_t
{
    Unknown = 0,
    Keyboard,
    Mouse,
    Paint,
    Command,   /**< Menu/toolbar .uno: slot dispatch */
    Document,  /**< UNO model mutation */
    Dialog,
    Lifecycle
};

/** Normalized event payload for AI agents — text-serializable. */
struct EventPayload
{
    EventCategory category = EventCategory::Unknown;
    std::string sourceModule;   /**< e.g. "sw", "sc", "sd" */
    std::string sourceFile;
    std::string sourceClass;
    std::string action;         /**< e.g. "KeyInput", "ExecPaste", "insertString" */
    std::string unoCommand;     /**< e.g. ".uno:Bold" — empty if N/A */
    std::string textData;       /**< AI-hookable text content */
    std::int64_t timestampMs = 0;
};

/**
 * AI Hooking interface — implement and register via EventListenerRegistry.
 * All methods are called on the main (SolarMutex) thread unless noted.
 */
class IEventListener
{
public:
    virtual ~IEventListener() = default;

    /** Human-readable listener id for logging and AI routing. */
    virtual std::string_view getListenerId() const = 0;

    /**
     * Called before the event is dispatched to VCL/SFX/model.
     * @return true to consume (block) the event; false to continue pipeline.
     */
    virtual bool onBeforeEvent(const EventPayload& rEvent) = 0;

    /** Called after the event has been processed. */
    virtual void onAfterEvent(const EventPayload& rEvent) = 0;

    /**
     * Optional text mutation hook — AI may rewrite rText before model commit.
     * @return true if rText was modified.
     */
    virtual bool onTextHook(std::string& rText, const EventPayload& rContext)
    {
        (void)rText;
        (void)rContext;
        return false;
    }
};

using EventListenerPtr = std::shared_ptr<IEventListener>;

/**
 * Singleton registry — thread-safe registration for AI hooking agents.
 * Integrates with SfxDispatcher / SwEditWin paths in future phases.
 */
class EventListenerRegistry
{
public:
    static EventListenerRegistry& instance();

    void addListener(EventListenerPtr pListener);
    void removeListener(std::string_view listenerId);
    void clear();

    /** Dispatch pre-event to all listeners; returns true if any consumed. */
    bool dispatchBefore(const EventPayload& rEvent);

    /** Dispatch post-event to all listeners. */
    void dispatchAfter(const EventPayload& rEvent);

    /** Text hook chain — each listener may mutate rText in order. */
    bool dispatchTextHook(std::string& rText, const EventPayload& rContext);

    std::size_t getListenerCount() const;

    EventListenerRegistry(const EventListenerRegistry&) = delete;
    EventListenerRegistry& operator=(const EventListenerRegistry&) = delete;

private:
    EventListenerRegistry() = default;

    mutable std::mutex m_mutex;
    std::vector<EventListenerPtr> m_listeners;
};

} // namespace lofice

#endif // INCLUDED_LOFICE_IEVENTLISTENER_HXX
