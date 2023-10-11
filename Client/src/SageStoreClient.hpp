#ifndef SAGESTORECLIENT_HPP
#define SAGESTORECLIENT_HPP

#include <memory>

#include "UI/UIManager.hpp"

class SageStoreClient
{
public:
    SageStoreClient();
    ~SageStoreClient();

    // Public methods
    void run();

private:
    std::unique_ptr<UIManager> m_uiManager;
};

#endif // SAGESTORECLIENT_HPP
