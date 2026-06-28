/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI Event Hooking API (implementation)
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

#include <lofice/IEventListener.hxx>

#include <algorithm>
#include <chrono>
#include <mutex>

namespace lofice
{

EventListenerRegistry& EventListenerRegistry::instance()
{
    static EventListenerRegistry s_instance;
    return s_instance;
}

void EventListenerRegistry::addListener(EventListenerPtr pListener)
{
    if (!pListener)
        return;
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto id = pListener->getListenerId();
    m_listeners.erase(
        std::remove_if(m_listeners.begin(), m_listeners.end(),
                       [&id](const EventListenerPtr& p) {
                           return p && p->getListenerId() == id;
                       }),
        m_listeners.end());
    m_listeners.push_back(std::move(pListener));
}

void EventListenerRegistry::removeListener(std::string_view listenerId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listeners.erase(
        std::remove_if(m_listeners.begin(), m_listeners.end(),
                       [&listenerId](const EventListenerPtr& p) {
                           return p && p->getListenerId() == listenerId;
                       }),
        m_listeners.end());
}

void EventListenerRegistry::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listeners.clear();
}

bool EventListenerRegistry::dispatchBefore(const EventPayload& rEvent)
{
    std::vector<EventListenerPtr> copy;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        copy = m_listeners;
    }
    for (const auto& pListener : copy)
    {
        if (pListener && pListener->onBeforeEvent(rEvent))
            return true;
    }
    return false;
}

void EventListenerRegistry::dispatchAfter(const EventPayload& rEvent)
{
    std::vector<EventListenerPtr> copy;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        copy = m_listeners;
    }
    for (const auto& pListener : copy)
    {
        if (pListener)
            pListener->onAfterEvent(rEvent);
    }
}

bool EventListenerRegistry::dispatchTextHook(std::string& rText, const EventPayload& rContext)
{
    std::vector<EventListenerPtr> copy;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        copy = m_listeners;
    }
    bool bModified = false;
    for (const auto& pListener : copy)
    {
        if (pListener && pListener->onTextHook(rText, rContext))
            bModified = true;
    }
    return bModified;
}

std::size_t EventListenerRegistry::getListenerCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_listeners.size();
}

} // namespace lofice
