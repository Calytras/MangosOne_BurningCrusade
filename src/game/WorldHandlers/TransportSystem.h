/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2025 MaNGOS <https://www.getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

/*
 * @addtogroup TransportSystem to provide abstract support for transported entities
 * The Transport System in MaNGOS consists of these files:
 * - TransportSystem.h to provide the basic classes TransportBase and TransportInfo
 * - TransportSystem.cpp which implements these classes
 * - Transports.h to implement the MOTransporter (subclas of gameobject) - Remains TODO
 * as well of
 * - impacts to various files
 *
 * @{
 *
 * @file TransportSystem.h
 * This file contains the headers for base clases needed for MaNGOS to handle passengers on transports
 *
 */

#ifndef _TRANSPORT_SYSTEM_H
#define _TRANSPORT_SYSTEM_H

#include "Common.h"
#include "Object.h"

class TransportInfo;

typedef UNORDERED_MAP < WorldObject* /*passenger*/, TransportInfo* /*passengerInfo*/ > PassengerMap;

/*
 * A class to provide basic support for each transporter. This includes
 * - Storing a list of passengers
 * - Providing helper for calculating between local and global coordinates
 */

class TransportBase
{
    public:
        explicit TransportBase(WorldObject* owner);
        virtual ~TransportBase();

        void Update(uint32 diff);
        void UpdateGlobalPositions();
        void UpdateGlobalPositionOf(WorldObject* passenger, float lx, float ly, float lz, float lo) const;

        WorldObject* GetOwner() const { return m_owner; }

        // Helper functions to calculate positions
        void RotateLocalPosition(float lx, float ly, float& rx, float& ry) const;
        void NormalizeRotatedPosition(float rx, float ry, float& lx, float& ly) const;

        void CalculateGlobalPositionOf(float lx, float ly, float lz, float lo, float& gx, float& gy, float& gz, float& go) const;

    protected:
        // Helper functions to add/ remove a passenger from the list
        void BoardPassenger(WorldObject* passenger, float lx, float ly, float lz, float lo);
        void UnBoardPassenger(WorldObject* passenger);

        WorldObject* m_owner;                               ///< The transporting unit
        PassengerMap m_passengers;                          ///< List of passengers and their transport-information

        // Helpers to speedup position calculations
        Position m_lastPosition;
        float m_sinO, m_cosO;
        uint32 m_updatePositionsTimer;                      ///< Timer that is used to trigger updates for global coordinate calculations
};

/*
 * A class to provide basic information for each transported passenger. This includes
 * - local positions
 * - Accessors to get the transporter
 */

class TransportInfo
{
    public:
        explicit TransportInfo(WorldObject* owner, TransportBase* transport, float lx, float ly, float lz, float lo);

        // Set local positions
        void SetLocalPosition(float lx, float ly, float lz, float lo);

        // Accessors
        WorldObject* GetTransport() const { return m_transport->GetOwner(); }
        ObjectGuid GetTransportGuid() const { return m_transport->GetOwner()->GetObjectGuid(); }

        // Get local position
        float GetLocalOrientation() const { return m_localPosition.o; }
        float GetLocalPositionX() const { return m_localPosition.x; }
        float GetLocalPositionY() const { return m_localPosition.y; }
        float GetLocalPositionZ() const { return m_localPosition.z; }
        void GetLocalPosition(float& lx, float& ly, float& lz, float& lo) const
        {
            lx = m_localPosition.x;
            ly = m_localPosition.y;
            lz = m_localPosition.z;
            lo = m_localPosition.o;
        }

    private:
        WorldObject* m_owner;                               ///< Passenger
        TransportBase* m_transport;                         ///< Transporter
        Position m_localPosition;
};

#endif
/*! @} */
