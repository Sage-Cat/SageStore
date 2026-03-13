#include <QSignalSpy>
#include <QtTest>

#include "Ui/Models/ProductTypesModel.hpp"

#include "common/Entities/ProductType.hpp"

#include "mocks/ApiManagerMock.hpp"

class ProductTypesModelTest : public QObject {
    Q_OBJECT

    ProductTypesModel *productTypesModel;
    ApiManagerMock *apiManagerMock;

public:
    ProductTypesModelTest()
    {
        apiManagerMock   = new ApiManagerMock();
        productTypesModel = new ProductTypesModel(*apiManagerMock);
    }

    ~ProductTypesModelTest() { delete productTypesModel; }

private slots:
    void testCreateProductType()
    {
        QSignalSpy createdSpy(productTypesModel, &ProductTypesModel::productTypeCreated);
        productTypesModel->createProductType(Common::Entities::ProductType{
            .id = "",
            .code = "PT-001",
            .barcode = "123",
            .name = "Oil",
            .description = "Synthetic oil",
            .lastPrice = "10.50",
            .unit = "pcs",
            .isImported = "0"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testEditProductType()
    {
        QSignalSpy editedSpy(productTypesModel, &ProductTypesModel::productTypeEdited);
        productTypesModel->editProductType(Common::Entities::ProductType{
            .id = "1",
            .code = "PT-001",
            .barcode = "123",
            .name = "Oil updated",
            .description = "Synthetic oil updated",
            .lastPrice = "11.00",
            .unit = "pcs",
            .isImported = "1"});
        QCOMPARE(editedSpy.count(), 1);
    }

    void testDeleteProductType()
    {
        QSignalSpy deletedSpy(productTypesModel, &ProductTypesModel::productTypeDeleted);
        productTypesModel->deleteProductType("1");
        QCOMPARE(deletedSpy.count(), 1);
    }

    void testFetchProductTypes()
    {
        QSignalSpy changedSpy(productTypesModel, &ProductTypesModel::productTypesChanged);
        productTypesModel->fetchProductTypes();
        QCOMPARE(changedSpy.count(), 1);
    }
};

QTEST_MAIN(ProductTypesModelTest)
#include "ProductTypesModelTest.moc"
