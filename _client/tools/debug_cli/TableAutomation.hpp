#pragma once

#include <QAbstractItemView>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDateEdit>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QLineEdit>
#include <QPointer>
#include <QTableWidget>
#include <QWidget>
#include <QtTest>

#include "OperationResult.hpp"

namespace DebugCli::TableAutomation {

inline void waitForInlineEditSettle(int delayMs = 150)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, delayMs);
    QTest::qWait(delayMs);
}

inline bool hasTableEditorAncestor(const QWidget *widget)
{
    const QWidget *current = widget;
    while (current != nullptr) {
        if (current->property("tableEditor").toBool()) {
            return true;
        }
        current = current->parentWidget();
    }

    return false;
}

inline QWidget *findEditorContainer(QWidget *widget)
{
    QWidget *current = widget;
    while (current != nullptr) {
        if (current->property("tableEditor").toBool()) {
            return current;
        }
        current = current->parentWidget();
    }

    return nullptr;
}

inline QWidget *findEditorWidget(QTableWidget *table)
{
    if (table == nullptr) {
        return nullptr;
    }

    if (auto *focusedWidget = QApplication::focusWidget()) {
        if (auto *container = findEditorContainer(focusedWidget); container != nullptr) {
            if (!container->property("tablePersistentEditor").toBool()) {
                return container;
            }
        }
    }

    const auto widgets = table->findChildren<QWidget *>();
    for (auto *widget : widgets) {
        if (widget->isVisible() && widget->property("tableEditor").toBool() &&
            !widget->property("tablePersistentEditor").toBool()) {
            return widget;
        }
    }

    return nullptr;
}

inline QComboBox *findPersistentComboEditor(QTableWidget *table, int row, int column)
{
    if (table == nullptr || row < 0 || column < 0) {
        return nullptr;
    }

    auto *item = table->item(row, column);
    if (item == nullptr) {
        return nullptr;
    }

    const QRect cellRect = table->visualItemRect(item);
    const QPoint cellCenter = cellRect.center();

    const auto combos = table->findChildren<QComboBox *>();
    for (auto *combo : combos) {
        if (!combo->isVisible() || !combo->property("tablePersistentEditor").toBool()) {
            continue;
        }

        const QPoint topLeft = combo->mapTo(table->viewport(), QPoint(0, 0));
        const QRect comboRect(topLeft, combo->size());
        if (!comboRect.contains(cellCenter)) {
            continue;
        }

        const bool rowMatches = combo->property("tableEditorRow").toInt() == row;
        const bool columnMatches = combo->property("tableEditorColumn").toInt() == column;
        if ((rowMatches && columnMatches) || !combo->property("tableEditorRow").isValid()) {
            return combo;
        }
    }

    return nullptr;
}

inline QLineEdit *findLineEditEditor(QTableWidget *table)
{
    if (table == nullptr) {
        return nullptr;
    }

    if (auto *focusedLineEdit = qobject_cast<QLineEdit *>(QApplication::focusWidget());
        focusedLineEdit != nullptr && hasTableEditorAncestor(focusedLineEdit)) {
        return focusedLineEdit;
    }

    const auto lineEdits = table->findChildren<QLineEdit *>();
    for (auto *lineEdit : lineEdits) {
        if (lineEdit->isVisible() && hasTableEditorAncestor(lineEdit)) {
            return lineEdit;
        }
    }

    return nullptr;
}

inline QWidget *activateEditor(QTableWidget *table, int row, int column)
{
    if (table == nullptr || row < 0 || column < 0 || table->item(row, column) == nullptr) {
        return nullptr;
    }

    if (auto *persistentCombo = findPersistentComboEditor(table, row, column);
        persistentCombo != nullptr) {
        table->setCurrentItem(table->item(row, column));
        table->selectRow(row);
        persistentCombo->setFocus(Qt::OtherFocusReason);
        return persistentCombo;
    }

    QPointer<QWidget> previousEditor = findEditorWidget(table);
    if (previousEditor != nullptr) {
        if (auto *lineEdit = qobject_cast<QLineEdit *>(previousEditor.data()); lineEdit != nullptr) {
            QTest::keyClick(lineEdit, Qt::Key_Enter);
        } else if (auto *comboBox = qobject_cast<QComboBox *>(previousEditor.data());
                   comboBox != nullptr) {
            QTest::keyClick(comboBox, Qt::Key_Enter);
        } else {
            previousEditor->clearFocus();
        }

        QElapsedTimer closeTimer;
        closeTimer.start();
        while (closeTimer.elapsed() < 1000) {
            if (previousEditor == nullptr || !previousEditor->isVisible()) {
                break;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
            QTest::qWait(10);
        }
    }

    if (auto *item = table->item(row, column); item != nullptr) {
        table->scrollToItem(item);
        table->setCurrentItem(item);
    }
    table->setFocus(Qt::OtherFocusReason);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 25);

    const auto *editableItem = table->item(row, column);
    if (editableItem == nullptr) {
        return nullptr;
    }

    const QRect itemRect = table->visualItemRect(editableItem);
    if (!itemRect.isValid()) {
        return nullptr;
    }

    QTest::mouseDClick(table->viewport(), Qt::LeftButton, Qt::NoModifier, itemRect.center());

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        if (auto *editor = findEditorWidget(table);
            editor != nullptr && editor != previousEditor.data()) {
            return editor;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    if (findEditorWidget(table) == nullptr) {
        if (auto *item = table->item(row, column); item != nullptr) {
            table->editItem(item);
        }
        if (findEditorWidget(table) == nullptr) {
            table->edit(table->model()->index(row, column));
        }
    }

    timer.restart();
    while (timer.elapsed() < 1000) {
        if (auto *editor = findEditorWidget(table);
            editor != nullptr && editor != previousEditor.data()) {
            return editor;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    return findEditorWidget(table);
}

inline OperationResult editTextCell(QTableWidget *table, int row, int column, const QString &text)
{
    QWidget *editor = activateEditor(table, row, column);
    if (editor == nullptr) {
        return OperationResult::failure(
            QStringLiteral("timeout"),
            QStringLiteral("Unable to activate an inline editor for row %1 column %2")
                .arg(row)
                .arg(column));
    }

    if (auto *dateEdit = qobject_cast<QDateEdit *>(editor); dateEdit != nullptr) {
        const QDate date = QDate::fromString(text, dateEdit->displayFormat());
        if (!date.isValid()) {
            return OperationResult::failure(
                QStringLiteral("assertion"),
                QStringLiteral("Invalid date '%1' for display format '%2'")
                    .arg(text, dateEdit->displayFormat()));
        }

        dateEdit->setDate(date);
        QTest::keyClick(dateEdit, Qt::Key_Enter);
        waitForInlineEditSettle();
        return OperationResult::success();
    }

    QWidget *inputTarget = findLineEditEditor(table);
    if (inputTarget == nullptr) {
        inputTarget = editor;
    }
    if (inputTarget == nullptr) {
        return OperationResult::failure(
            QStringLiteral("timeout"),
            QStringLiteral("Unable to locate the inline editor input widget"));
    }

    if (auto *lineEdit = qobject_cast<QLineEdit *>(inputTarget); lineEdit != nullptr) {
        lineEdit->selectAll();
    } else if (auto *spinBox = qobject_cast<QAbstractSpinBox *>(editor); spinBox != nullptr) {
        spinBox->selectAll();
    } else {
        inputTarget->setFocus(Qt::OtherFocusReason);
        QTest::keyClick(inputTarget, Qt::Key_A, Qt::ControlModifier);
    }

    QTest::keyClicks(inputTarget, text);
    QTest::keyClick(inputTarget, Qt::Key_Enter);

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        if (findEditorWidget(table) == nullptr) {
            waitForInlineEditSettle();
            return OperationResult::success();
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    return OperationResult::failure(QStringLiteral("timeout"),
                                    QStringLiteral("Inline editor did not close after commit"));
}

inline QComboBox *comboCell(QTableWidget *table, int row, int column)
{
    if (table == nullptr || row < 0 || column < 0) {
        return nullptr;
    }

    if (auto *persistentCombo = findPersistentComboEditor(table, row, column);
        persistentCombo != nullptr) {
        table->setCurrentItem(table->item(row, column));
        table->selectRow(row);
        return persistentCombo;
    }

    activateEditor(table, row, column);

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        if (auto *focusedCombo = qobject_cast<QComboBox *>(QApplication::focusWidget());
            focusedCombo != nullptr && focusedCombo->property("tableEditor").toBool()) {
            return focusedCombo;
        }

        const auto combos = table->findChildren<QComboBox *>();
        for (auto *combo : combos) {
            if (combo->isVisible() && combo->property("tableEditor").toBool()) {
                return combo;
            }
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    return nullptr;
}

inline OperationResult setComboCellByText(QTableWidget *table, int row, int column,
                                          const QString &text)
{
    QComboBox *combo = comboCell(table, row, column);
    if (combo == nullptr) {
        return OperationResult::failure(
            QStringLiteral("timeout"),
            QStringLiteral("Unable to locate a combo box editor for row %1 column %2")
                .arg(row)
                .arg(column));
    }

    const int index = combo->findText(text);
    if (index < 0) {
        return OperationResult::failure(
            QStringLiteral("assertion"),
            QStringLiteral("Combo value '%1' not found for row %2 column %3")
                .arg(text)
                .arg(row)
                .arg(column));
    }

    const bool persistent = combo->property("tablePersistentEditor").toBool();
    if (persistent) {
        combo->setFocus(Qt::OtherFocusReason);
        combo->setCurrentIndex(index);
        QMetaObject::invokeMethod(combo, "activated", Qt::DirectConnection, Q_ARG(int, index));
        waitForInlineEditSettle();
        return OperationResult::success();
    }

    combo->showPopup();
    if (combo->view() == nullptr) {
        return OperationResult::failure(QStringLiteral("infra"),
                                        QStringLiteral("Combo box has no popup view"));
    }

    combo->view()->setCurrentIndex(combo->model()->index(index, combo->modelColumn()));
    const QRect itemRect = combo->view()->visualRect(combo->view()->currentIndex());
    QTest::mouseClick(combo->view()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      itemRect.center());

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        if (findEditorWidget(table) == nullptr) {
            waitForInlineEditSettle();
            return OperationResult::success();
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    return OperationResult::failure(QStringLiteral("timeout"),
                                    QStringLiteral("Combo editor did not close after commit"));
}

} // namespace DebugCli::TableAutomation
