#include "DatabaseManager.h"
#include <memory>
#include <string>
#include <iostream>




void DatabaseManager::InitDB()
{
    m_authPool.Start(2, "192.168.1.13", "gquestdev", "Apple1", "auth_gquest");
    m_characterPool.Start(2, "192.168.1.13", "gquestdev", "Apple1", "character_gquest");
    m_contentPool.Start(2, "192.168.1.13", "gquestdev", "Apple1", "content_gquest");
}

void DatabaseManager::ShutdownDB()
{
    m_authPool.Stop();
    m_characterPool.Stop();
    m_contentPool.Stop();
}
