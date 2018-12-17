#include "LobbyTableView.h"
#include <iostream>
#include "GlobalVariable.h"
#include "SFSConnection.h"


// ---------------------------------------------------------
// LobbyListDelegate
// ---------------------------------------------------------
void LobbyListDelegate::tableCellTouched(TableView* table, TableViewCell* cell)
{
	using namespace std;
	CustomTableViewCell* customCell = static_cast<CustomTableViewCell*>(cell);
	Layout* layoutItem = customCell->m_pLayout;
	TextField* txtRoomName = static_cast<TextField*>(layoutItem->getChildByName("txtRoomName"));
	TextField* txtRoomNumber = static_cast<TextField*>(layoutItem->getChildByName("txtRoomNumber"));
	string str = txtRoomName->getString();
	int tableId = atoi(txtRoomNumber->getString().c_str());
	GlobalVariable::m_iCurrentTable = tableId;
	str += ccsf("%d", tableId);
	SFSConnection::getInstance()->joinRoom(str);
}


// ---------------------------------------------------------
// LobbyDataSource
// ---------------------------------------------------------
LobbyDataSource::LobbyDataSource(){}
LobbyDataSource::~LobbyDataSource(){}

TableViewCell* LobbyDataSource::tableCellAtIndex(TableView* table, ssize_t index)
{
	using namespace cocos2d::ui;
	CustomTableViewCell* cell = static_cast<CustomTableViewCell*>(table->dequeueCell());
	if (!cell) {
		cell = new CustomTableViewCell();
		cell->autorelease();

		cocos2d::ui::Layout* item = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(kItemListRoom));
		item->setTouchEnabled(false);
		cocos2d::ui::ImageView* bg = dynamic_cast<cocos2d::ui::ImageView*>(cocos2d::ui::Helper::seekWidgetByName(item, "imgBg"));
		item->removeChild(bg);
		cell->addChild(bg);
		cell->addChild(item);
		cell->setAnchorPoint(Vec2::ZERO);
		cell->m_pLayout = item;
		cell->m_bg = bg;
	}

	if (index < GlobalVariable::m_vTableData->size())
    {
        auto data = GlobalVariable::m_vTableData;
		cocos2d::ui::Layout* item = cell->m_pLayout;
		TextField* txtRoomNumber = static_cast<TextField*>(item->getChildByName("txtRoomNumber"));
		txtRoomNumber->setString(ccsf("%ld", GlobalVariable::m_vTableData->at(index)->m_iNumber));

		TextField* txtRoomNameIn = static_cast<TextField*>(item->getChildByName("txtRoomName"));
		txtRoomNameIn->setString(GlobalVariable::m_vTableData->at(index)->m_sName.c_str());
		txtRoomNameIn->setVisible(false);

		TextField* txtRoomTitle = static_cast<TextField*>(item->getChildByName("txtRoomTitle"));
		int indexMsg = rand() % 9 + 1;

		string sMsg = ccsf("TABLE_MESS_%d", indexMsg);
		txtRoomTitle->setString(ResourceManager::getInstance()->getTextByName(sMsg.c_str()));
		TextField* txtStakes = static_cast<TextField*>(item->getChildByName("txtRoomBet"));
		txtStakes->setString(TTutil::addDot(ccsf("%ld", GlobalVariable::m_vTableData->at(index)->m_iCuoc)));

		Layout* pnlPlayer = static_cast<Layout*>(item->getChildByName("pnlPlayer"));
		for (int ii = 1; ii <= 4; ii++)
        {
			std::string str = ccsf("imgPlayer%d", ii);
			ImageView* imgPlayer = static_cast<ImageView*>(pnlPlayer->getChildByName(str.c_str()));
			ImageView* imgTaken = static_cast<ImageView*>(imgPlayer->getChildByName("imgTaken"));
			if (ii <= GlobalVariable::m_vTableData->at(index)->m_iNumPlayers)
            {
				imgTaken->setVisible(true);
			}
			else
            {
				imgTaken->setVisible(false);
			}
		}
		cell->m_bg->setVisible(index % 2 == 0 ? false : true);
	}

	return cell;
}

ssize_t LobbyDataSource::numberOfCellsInTableView(TableView* table)
{
	int noTable = GlobalVariable::m_vTableData->size();
	return noTable;
}

Size LobbyDataSource::tableCellSizeForIndex(TableView* tableView, ssize_t idx)
{
	return Size(760, 60);
}


// ---------------------------------------------------------
// CustomTableViewCell
// ---------------------------------------------------------
void CustomTableViewCell::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	TableViewCell::draw(renderer, transform, flags);
}
