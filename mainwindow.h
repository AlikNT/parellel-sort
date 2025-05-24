#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void runSorts();

private:
    std::vector<int> generateData(size_t size);
    void quickSort(std::vector<int>& arr, int left, int right);
    void heapSort(std::vector<int>& arr);
    void heapify(std::vector<int>& arr, int n, int i);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
