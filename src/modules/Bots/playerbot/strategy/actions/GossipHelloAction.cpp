#include "botpch.h"
#include "../../playerbot.h"
#include "GossipHelloAction.h"

using namespace ai;

/**
 * @brief Execute the Gossip Hello action.
 *
 * @param event The event triggering the action.
 * @return true If the action was executed successfully.
 * @return false Otherwise.
 */
bool GossipHelloAction::Execute(Event event)
{
    ObjectGuid guid;

    WorldPacket &p = event.getPacket();
    if (p.empty())
    {
        Player* master = GetMaster();
        if (master)
        {
            guid = master->GetSelectionGuid();
        }
    }
    else
    {
        p.rpos(0);
        p >> guid;
    }

    if (!guid)
    {
        return false;
    }

    Creature *pCreature = bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
    if (!pCreature)
    {
        DEBUG_LOG("[PlayerbotMgr]: HandleMasterIncomingPacket - Received  CMSG_GOSSIP_HELLO %s not found or you can't interact with him.", guid.GetString().c_str());
        return false;
    }

    GossipMenuItemsMapBounds pMenuItemBounds = sObjectMgr.GetGossipMenuItemsMapBounds(pCreature->GetCreatureInfo()->GossipMenuId);
    if (pMenuItemBounds.first == pMenuItemBounds.second)
    {
        return false;
    }

    string text = event.getParam();
    int menuToSelect = -1;
    if (text.empty())
    {
        WorldPacket p1;
        p1 << guid;
        bot->GetSession()->HandleGossipHelloOpcode(p1);
        bot->SetFacingToObject(pCreature);

        ostringstream out; out << "--- " << pCreature->GetName() << " ---";
        ai->TellMasterNoFacing(out.str());

        TellGossipMenus();
    }
    else if (!bot->PlayerTalkClass)
    {
        ai->TellMaster("I need to talk first");
        return false;
    }
    else
    {
        menuToSelect = atoi(text.c_str());
        if (menuToSelect > 0) menuToSelect--;
        {
            ProcessGossip(menuToSelect);
        }
    }

    bot->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    return true;
}

/**
 * @brief Tell the gossip text to the master.
 *
 * @param textId The ID of the gossip text.
 */
void GossipHelloAction::TellGossipText(uint32 textId)
{
    if (!textId)
    {
        return;
    }

    GossipText const* text = sObjectMgr.GetGossipText(textId);
    if (text)
    {
        for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; i++)
        {
            string text0 = text->Options[i].Text_0;
            if (!text0.empty())
            {
                ai->TellMasterNoFacing(text0);
            }
            string text1 = text->Options[i].Text_1;
            if (!text1.empty())
            {
                ai->TellMasterNoFacing(text1);
            }
        }
    }
}

/**
 * @brief Tell the gossip menus to the master.
 */
void GossipHelloAction::TellGossipMenus()
{
    if (!bot->PlayerTalkClass)
    {
        return;
    }

    Creature *pCreature = bot->GetNPCIfCanInteractWith(GetMaster()->GetSelectionGuid(), UNIT_NPC_FLAG_NONE);
    GossipMenu& menu = bot->PlayerTalkClass->GetGossipMenu();
    if (pCreature)
    {
        uint32 textId = bot->GetGossipTextId(menu.GetMenuId(), pCreature);
        TellGossipText(textId);
    }

    for (int i = 0; i < menu.MenuItemCount(); i++)
    {
        GossipMenuItem const& item = menu.GetItem(i);
        ostringstream out; out << "[" << (i+1) << "] " << item.m_gMessage;
        ai->TellMasterNoFacing(out.str());
    }
}

/**
 * @brief Process the selected gossip menu option.
 *
 * @param menuToSelect The index of the menu option to select.
 * @return true If the gossip option was processed successfully.
 * @return false Otherwise.
 */
bool GossipHelloAction::ProcessGossip(int menuToSelect)
{
    GossipMenu& menu = bot->PlayerTalkClass->GetGossipMenu();
    if (menuToSelect != -1 && menuToSelect >= menu.MenuItemCount())
    {
        ai->TellMaster("Unknown gossip option");
        return false;
    }
    if (menuToSelect == -1)
    {
        return false;
    }
    GossipMenuItem const& item = menu.GetItem(menuToSelect);
    WorldPacket p;
    std::string code;
    p << GetMaster()->GetSelectionGuid() << menuToSelect << code;
    bot->GetSession()->HandleGossipSelectOptionOpcode(p);

    TellGossipMenus();
    return true;
}

