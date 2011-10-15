/* 
 * Copyright (C) 2011 Jordi Mas i Hern�ndez <jmas@softcatala.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */

#include "stdafx.h"
#include "ApplicationsPropertyPageUI.h"
#include "Actions.h"
#include "Action.h"

#include <vector>
using namespace std;

static WNDPROC PreviousProc;

LRESULT ApplicationsPropertyPageUI::ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {		
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		{
			LVHITTESTINFO lvHitTestInfo;
			lvHitTestInfo.pt.x = LOWORD(lParam);
			lvHitTestInfo.pt.y = HIWORD(lParam);
			int nItem = ListView_HitTest(hWnd, &lvHitTestInfo);

			if (lvHitTestInfo.flags & LVHT_ONITEMSTATEICON)
			{
				LVITEM item;

				memset(&item,0,sizeof(item));
				item.iItem = nItem;
				item.mask = LVIF_PARAM;			
				ListView_GetItem(hWnd, &item);
				Action* action = (Action *) item.lParam;			
				if (action->IsNeed () == false)
				{					
					return 0;
				}
			}
		}
		
		case WM_KEYDOWN:
		{
			if (wParam == VK_SPACE)
			{
				LVITEM item;

				memset(&item,0,sizeof(item));
				item.iItem = ListView_GetSelectionMark(hWnd);
				item.mask = LVIF_PARAM;			
				ListView_GetItem(hWnd, &item);
				Action* action = (Action *) item.lParam;			
				if (action->IsNeed () == false)
				{					
					return 0;
				}
			}
		}

		default:
			return CallWindowProc(PreviousProc, hWnd, uMsg, wParam, lParam); 
	}
} 

void ApplicationsPropertyPageUI::_onInitDialog()
{
	int nItemId = 0;
	hList = GetDlgItem(getHandle(), IDC_APPLICATIONSLIST);

	ListView_SetExtendedListViewStyle (hList, LVS_EX_CHECKBOXES);

	LVITEM item;
	memset(&item,0,sizeof(item));
	item.mask=LVIF_TEXT | LVIF_PARAM;

	Actions actions;
	m_actions = actions.GetActions ();

	if (m_actions.size () == 0)
		return;

	// Enabled items
	for (unsigned int i = 0; i < m_actions.size (); i++)
	{		
		Action* action = m_actions.at(i);
		bool needed = action->IsNeed();		

		if (needed == false)
			continue;

		m_disabledActions.insert(ActionBool_Pair (action, needed));
		item.iItem=nItemId;
		item.pszText= action->GetName ();
		item.lParam = (LPARAM) action;		
		ListView_InsertItem (hList, &item);
		ListView_SetCheckState (hList, nItemId, true);
		nItemId++;
	}

	// Disabled items
	for (unsigned int i = 0; i < m_actions.size (); i++)
	{		
		Action* action = m_actions.at(i);
		bool needed = action->IsNeed();		

		if (needed == true)
			continue;

		m_disabledActions.insert(ActionBool_Pair (action, needed));
		item.iItem=nItemId;
		item.pszText= action->GetName ();
		item.lParam = (LPARAM) action;		
		ListView_InsertItem (hList, &item);
		ListView_SetCheckState (hList, nItemId, false);
		nItemId++;
	}

	ListView_SetItemState (hList, 0, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
	PreviousProc = (WNDPROC)SetWindowLongPtr (hList, GWLP_WNDPROC, (LONG_PTR) ListViewSubclassProc);
}

NotificationResult ApplicationsPropertyPageUI::_onNotify(LPNMHDR hdr, int iCtrlID)
{
	if(hdr->idFrom != IDC_APPLICATIONSLIST)
		return ReturnFalse;

	if (hdr->code == NM_CUSTOMDRAW)
	{
		LPNMLVCUSTOMDRAW lpNMLVCD = (LPNMLVCUSTOMDRAW)hdr;

		if (lpNMLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		{
			SetWindowLong (getHandle (), DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
			return ReturnTrue;
		}

		if (lpNMLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		{
			Action* action = (Action *)  lpNMLVCD->nmcd.lItemlParam;
			map <Action *, bool>::iterator item;

			item = m_disabledActions.find((Action * const &)action);
			bool enabled = item->second;

			if (enabled == false)
			{
				DWORD color = GetSysColor(COLOR_GRAYTEXT);
				lpNMLVCD->clrText = color;				
			}
			SetWindowLong (getHandle (), DWLP_MSGRESULT, CDRF_DODEFAULT);
			return ReturnTrue;
		}
		
		return CallDefProc;		
	}

	NMLISTVIEW *pListView = (NMLISTVIEW *)hdr;
	    
	if(pListView->hdr.code != LVN_ITEMCHANGED)
		return ReturnFalse;

	Action* action = (Action *)  pListView->lParam;

	SendDlgItemMessage (getHandle(), IDC_APPLICATION_DESCRIPTION,
		WM_SETTEXT, (WPARAM) 0, 
		(LPARAM) action->GetDescription());

	return ReturnTrue;
}

void ApplicationsPropertyPageUI::_onNext()
{
	int items = ListView_GetItemCount (hList);

	for (int i = 0; i < items; ++i)
	{
		if (ListView_GetCheckState(hList, i) == FALSE)
			continue;

		LVITEM item;

		memset(&item,0,sizeof(item));
		item.iItem = i;
		item.mask = LVIF_PARAM;

		ListView_GetItem(hList, &item);
		m_selectedActions->push_back((Action *) item.lParam);
	}	
}