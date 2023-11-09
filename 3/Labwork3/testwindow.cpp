#include <QPair>
#include <QDialog>
#include <QCheckBox>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>

#include "testwindow.h"
#include "ui_testwindow.h"

#include "testdata.h"
#include "testmanager.h"

TestWindow::TestWindow(QWidget* parent, const QString& filePath) : QDialog(parent), ui(new Ui::TestWindow)
{
    const int TEST_WINDOW_WIDTH = 500;
    const int TEST_WINDOW_HEIGHT = 500;

    const int BUTTON_WIDTH = 100;
    const int BUTTON_HEIGHT = 50;
    const int BUTTON_OFFSET = 100;

    this->ui->setupUi(this);

    this->setMinimumSize(TEST_WINDOW_WIDTH, TEST_WINDOW_HEIGHT);
    this->setMaximumSize(TEST_WINDOW_WIDTH, TEST_WINDOW_HEIGHT);

    this->layout = new QVBoxLayout(this);
    this->labelQuestion = new QLabel(this);
    this->buttonNext = new QPushButton(this);
    this->buttonPrevious = new QPushButton(this);

    this->ui->verticalLayout->addLayout(this->layout);
    this->ui->verticalLayout->addWidget(this->labelQuestion);

    this->labelQuestion->setWordWrap(true);
    this->labelQuestion->setFont(QFont(this->labelQuestion->font().family(), 14));

    this->buttonPrevious->setGeometry(0, TEST_WINDOW_HEIGHT - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT);
    this->buttonNext->setGeometry(TEST_WINDOW_WIDTH - BUTTON_OFFSET, TEST_WINDOW_HEIGHT - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT);

    this->buttonNext->setText("Далі");
    this->buttonNext->setFont(QFont(this->buttonNext->font().family(), 12));

    this->buttonPrevious->setText("Назад");
    this->buttonPrevious->setFont(QFont(this->buttonPrevious->font().family(), 12));

    connect(this->buttonNext, &QPushButton::clicked, this, &TestWindow::onButtonNextClicked);
    connect(this->buttonPrevious, &QPushButton::clicked, this, &TestWindow::onButtonPreviousClicked);

    TestManager::getInstance().loadTests(filePath);

    this->testIndex = 0;
    this->displayTest(testIndex);
}

void TestWindow::displayTest(const int index)
{
    if (index >= 0 && index < TestManager::getInstance().getTestsCount())
    {
        this->deleteTestUI();

        TestData test = TestManager::getInstance().getTest(testIndex);

        this->labelQuestion->setText(test.question);

        for (QPair<QPair<QString, bool>, bool>& answer : test.answers)
        {
            QCheckBox* checkBox = new QCheckBox(answer.first.first);
            checkBox->setFont(QFont(checkBox->font().family(), 14));
            checkBox->setChecked(answer.second);
            this->layout->addWidget(checkBox);
            connect(checkBox, &QCheckBox::toggled, this, [=, &answer](bool checked) { answer.second = checked; });
        }

        this->layout->addStretch();
        this->adjustSize();

        TestManager::getInstance().updateTestResults(index, test);
    }
    else
        QMessageBox::critical(this, "Помилка", "Неправильний індекс тесту.");
}

void TestWindow::onButtonNextClicked(void)
{
    if (this->testIndex + 1 < TestManager::getInstance().getTestsCount())
    {
        this->displayTest(++this->testIndex);
    }
    else
    {
        int result = QMessageBox::question(this, "Question", "Do you want to close the test and show the result?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (result == QMessageBox::Yes)
            QMessageBox::information(this, "Результат", QString("Total Result: %1").arg(TestManager::getInstance().calculateTestTotalResults()));
        else
            this->displayTest(this->testIndex);
    }
}

void TestWindow::onButtonPreviousClicked(void)
{
    if (this->testIndex > 0)
        this->displayTest(--this->testIndex);
}

void TestWindow::deleteTestUI(void) const
{
    QLayoutItem* item;

    while ((item = layout->takeAt(0)) != nullptr)
    {
        if (item->widget())
            delete item->widget();

        delete item;
    }
}

TestWindow::~TestWindow(void)
{
    this->deleteTestUI();

    delete this->ui;
    this->ui = nullptr;

    delete this->layout;
    this->layout = nullptr;

    delete this->labelQuestion;
    this->labelQuestion = nullptr;

    delete this->buttonNext;
    this->buttonNext = nullptr;

    delete this->buttonPrevious;
    this->buttonPrevious = nullptr;
}
