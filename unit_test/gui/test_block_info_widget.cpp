#include <QtTest/QtTest>
#include <QApplication>

#include "block_info_widget.hpp"

namespace test
{

class SignalTestBlockInfoWidget : public QObject
{
  Q_OBJECT
public:
  QString name() const;
  QString description() const;
  void Reset();

public slots:
  void OnUpdated(const QString& name, const QString& description);

private:
  QString name_;
  QString description_;
};

QString SignalTestBlockInfoWidget::name() const
{
  return name_;
}

QString SignalTestBlockInfoWidget::description() const
{
  return description_;
}

void SignalTestBlockInfoWidget::OnUpdated(const QString& name,
                                          const QString& description)
{
  name_ = name;
  description_ = description;
}

void SignalTestBlockInfoWidget::Reset()
{
  name_.clear();
  description_.clear();
}

class TestBlockInfoWidget : public QObject
{
  Q_OBJECT
private slots:
  void TestContent();
};

void TestBlockInfoWidget::TestContent()
{
  QString block_name = tr("test_block");
  QString block_description = tr("This is a test block!\nJust for testing.");
  hs::recon::BlockInfoWidget block_info_widget(block_name, block_description);
  SignalTestBlockInfoWidget signal_tester;
  QObject::connect(&block_info_widget, &hs::recon::BlockInfoWidget::UpdateBlock,
                   &signal_tester, &SignalTestBlockInfoWidget::OnUpdated);
  block_info_widget.show();

  QWidget* button = block_info_widget.childAt(200, 270);
  if (button)
  {
    QTest::mouseClick(button, Qt::LeftButton, 0);

    QCOMPARE(signal_tester.name(), block_name);
    QCOMPARE(signal_tester.description(), block_description);
  }
  else
  {
    QFAIL("Can't find button!");
  }
}

}

QTEST_MAIN(test::TestBlockInfoWidget);
#include "test_block_info_widget.moc"

