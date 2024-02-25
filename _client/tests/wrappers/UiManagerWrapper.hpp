#pragma once

#include "Ui/UiManager.hpp"

// TODO: implement mock for DialogManager and MainWindow

class QApplication;
class ApiManager;

class UiManagerWrapper : public UiManager
{
public:
    UiManagerWrapper(QApplication &app, ApiManager &apiManager) : UiManager(app, apiManager)
    {
    }
};