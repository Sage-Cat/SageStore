#include <QSignalSpy>
#include <QtTest>

#include "Ui/Models/StocksModel.hpp"

#include "common/Entities/Inventory.hpp"

#include "mocks/ApiManagerMock.hpp"

class StocksModelTest : public QObject {
    Q_OBJECT

    StocksModel *stocksModel{nullptr};
    ApiManagerMock *apiManagerMock{nullptr};

private slots:
    void init()
    {
        apiManagerMock = new ApiManagerMock();
        stocksModel    = new StocksModel(*apiManagerMock);
    }

    void cleanup()
    {
        delete stocksModel;
        delete apiManagerMock;
        stocksModel    = nullptr;
        apiManagerMock = nullptr;
    }

    void testCreateStock()
    {
        QSignalSpy createdSpy(stocksModel, &StocksModel::stockCreated);
        stocksModel->createStock(Common::Entities::Inventory{
            .id = "", .productTypeId = "1", .quantityAvailable = "13", .employeeId = "1"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testEditStock()
    {
        QSignalSpy editedSpy(stocksModel, &StocksModel::stockEdited);
        stocksModel->editStock(Common::Entities::Inventory{
            .id = "1", .productTypeId = "1", .quantityAvailable = "18", .employeeId = "1"});
        QCOMPARE(editedSpy.count(), 1);
    }

    void testDeleteStock()
    {
        QSignalSpy deletedSpy(stocksModel, &StocksModel::stockDeleted);
        stocksModel->deleteStock("1");
        QCOMPARE(deletedSpy.count(), 1);
    }

    void testFetchStocks()
    {
        QSignalSpy changedSpy(stocksModel, &StocksModel::stocksChanged);
        stocksModel->fetchStocks();
        QCOMPARE(changedSpy.count(), 1);
    }
};

QTEST_MAIN(StocksModelTest)
#include "StocksModelTest.moc"
