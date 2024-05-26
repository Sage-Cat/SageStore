#include <QSignalSpy>
#include <QtTest>

#include "Ui/Models/UsersManagementModel.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"

#include "mocks/ApiManagerMock.hpp"

class UsersManagementModelTest : public QObject {
    Q_OBJECT

    UsersManagementModel *usersManagementModel;
    ApiManagerMock *apiManagerMock;

public:
    UsersManagementModelTest()
    {
        apiManagerMock       = new ApiManagerMock();
        usersManagementModel = new UsersManagementModel(*apiManagerMock);
    }

    ~UsersManagementModelTest() { delete usersManagementModel; }

private slots:
    void testAddUser()
    {
        QSignalSpy userAddedSpy(usersManagementModel, &UsersManagementModel::userAdded);
        usersManagementModel->addUser(
            User{.id = "1", .username = "newUser", .password = "newPassword", .roleId = "1"});
        QCOMPARE(userAddedSpy.count(), 1);
    }

    void testEditUser()
    {
        QSignalSpy userEditedSpy(usersManagementModel, &UsersManagementModel::userEdited);
        usersManagementModel->editUser(User{
            .id = "1", .username = "updatedUser", .password = "updatedPassword", .roleId = "1"});
        QCOMPARE(userEditedSpy.count(), 1);
    }

    void testDeleteUser()
    {
        QSignalSpy userDeletedSpy(usersManagementModel, &UsersManagementModel::userDeleted);
        usersManagementModel->deleteUser("1");
        QCOMPARE(userDeletedSpy.count(), 1);
    }

    void testFetchUsers()
    {
        QSignalSpy usersChangedSpy(usersManagementModel, &UsersManagementModel::usersChanged);
        usersManagementModel->fetchUsers();
        QCOMPARE(usersChangedSpy.count(), 1);
    }

    void testCreateRole()
    {
        QSignalSpy roleCreatedSpy(usersManagementModel, &UsersManagementModel::roleCreated);
        usersManagementModel->createRole(Role{.name = "testRole"});
        QCOMPARE(roleCreatedSpy.count(), 1);
    }

    void testEditRole()
    {
        QSignalSpy roleEditedSpy(usersManagementModel, &UsersManagementModel::roleEdited);
        usersManagementModel->editRole(Role{.id = "1", .name = "updatedRole"});
        QCOMPARE(roleEditedSpy.count(), 1);
    }

    void testDeleteRole()
    {
        QSignalSpy roleDeletedSpy(usersManagementModel, &UsersManagementModel::roleDeleted);
        usersManagementModel->deleteRole("1");
        QCOMPARE(roleDeletedSpy.count(), 1);
    }

    void testFetchRoles()
    {
        QSignalSpy rolesChangedSpy(usersManagementModel, &UsersManagementModel::rolesChanged);
        usersManagementModel->fetchRoles();
        QCOMPARE(rolesChangedSpy.count(), 1);
    }

    void testErrorOccurred()
    {
        QSignalSpy errorOccurredSpy(usersManagementModel, &UsersManagementModel::errorOccurred);
        apiManagerMock->emitError("Test error");
        QCOMPARE(errorOccurredSpy.count(), 1);
    }
};

QTEST_MAIN(UsersManagementModelTest)
#include "UsersManagementModelTest.moc"
