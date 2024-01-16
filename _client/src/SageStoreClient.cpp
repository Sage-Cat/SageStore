#include "SageStoreClient.hpp"

#include <QFont>

#include "SpdlogWrapper.hpp"

#include "UiManager.hpp"

#include "Models/AuthorizationModel.hpp"
#include "Models/RegistrationModel.hpp"

#include "ViewModels/AuthorizationViewModel.hpp"
#include "ViewModels/RegistrationViewModel.hpp"

SageStoreClient::SageStoreClient(QApplication &app) : m_app(app)
{
    SPDLOG_TRACE("SageStoreClient::SageStoreClient");

    // Initialize all necessary elements
    init();
    setupMVMConnections();
    m_uiManager->initiateAuthorizationProcess();

    // QApplication settings
    applyAppFont();

    // Log lifecycle ending
    connect(&m_app, &QCoreApplication::aboutToQuit, []()
            { SPDLOG_INFO("SageStoreClient finished with code=0"); });
}

SageStoreClient::~SageStoreClient()
{
    SPDLOG_TRACE("SageStoreClient::~SageStoreClient");
}

void SageStoreClient::init()
{
    SPDLOG_TRACE("SageStoreClient::init");

    // UiManager
    m_uiManager = new UiManager(this);

    // Models
    initModels();
}

void SageStoreClient::initModels()
{
    SPDLOG_TRACE("SageStoreClient::initModels");

    m_authorizationModel = new AuthorizationModel;
    m_registrationModel = new RegistrationModel;
}

void SageStoreClient::setupMVMConnections()
{
    SPDLOG_TRACE("SageStoreClient::setupMVMConnections");

    if (m_uiManager)
    {
        // Authorization
        connect(m_uiManager->authorizationViewModel(), &AuthorizationViewModel::requestAuthentication,
                m_authorizationModel, &AuthorizationModel::onAuthenticationRequested);
        connect(m_authorizationModel, &AuthorizationModel::authenticationSuccessful,
                m_uiManager->authorizationViewModel(), &AuthorizationViewModel::loginSuccessful);
        connect(m_authorizationModel, &AuthorizationModel::authenticationFailed,
                m_uiManager->authorizationViewModel(), &AuthorizationViewModel::loginFailed);

        // Registration
        connect(m_uiManager->registrationViewModel(), &RegistrationViewModel::requestRegistration,
                m_registrationModel, &RegistrationModel::onRegistrationRequested);
        connect(m_registrationModel, &RegistrationModel::registrationSuccessful,
                m_uiManager->registrationViewModel(), &RegistrationViewModel::registrationSuccessful);
        connect(m_registrationModel, &RegistrationModel::registrationFailed,
                m_uiManager->registrationViewModel(), &RegistrationViewModel::registrationFailed);
    }
    else
    {
        SPDLOG_ERROR("SageStoreClient::setupMVMConnections m_uiManager is not initialized");
    }
}

void SageStoreClient::applyAppFont()
{
    const QFont font = m_uiManager->defaultFont();
    m_app.setFont(font);
    SPDLOG_INFO("Application font set to {}", font.toString().toStdString());
}