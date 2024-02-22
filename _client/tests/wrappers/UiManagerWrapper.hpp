#pragma once

#include "Ui/UiManager.hpp"

#include "DialogManagerMock.hpp"

class QApplication;
class ApiManager;

class UiManagerWrapper : public UiManager
{
public:
    UiManagerWrapper(QApplication &app, ApiManager &apiManager) : UiManager(app, apiManager)
    {
    }

    void init() override
    {
        UiManager::init();

        delete m_dialogManager;
        m_dialogManager = new DialogManagerMock(m_apiManager);
    }
};