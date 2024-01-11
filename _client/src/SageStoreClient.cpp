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

    // QApplication styles
    m_app.setFont(m_uiManager->defaultFont());

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

    m_uiManager = new UiManager(this);
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