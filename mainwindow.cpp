#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtConcurrentRun>
#include <QElapsedTimer>
#include <QFuture>
#include <QDebug>
#include <algorithm>
#include <random>

const int PARALLEL_THRESHOLD = 10000;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::runSorts);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<int> MainWindow::generateData(size_t size) {
    std::vector<int> data(size);
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 1000000);
    std::generate(data.begin(), data.end(), [&] { return dist(gen); });
    return data;
}

void MainWindow::quickSort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (arr[i] < pivot) ++i;
        while (arr[j] > pivot) --j;
        if (i <= j) std::swap(arr[i++], arr[j--]);
    }

    int leftSize = j - left + 1;
    int rightSize = right - i + 1;
    QFuture<void> futureLeft, futureRight;

    if (leftSize > PARALLEL_THRESHOLD)
        futureLeft = QtConcurrent::run([&arr, left, j, this] { quickSort(arr, left, j); });
    else
        quickSort(arr, left, j);

    if (rightSize > PARALLEL_THRESHOLD)
        futureRight = QtConcurrent::run([&arr, i, right, this] { quickSort(arr, i, right); });
    else
        quickSort(arr, i, right);

    if (leftSize > PARALLEL_THRESHOLD) futureLeft.waitForFinished();
    if (rightSize > PARALLEL_THRESHOLD) futureRight.waitForFinished();
}

void MainWindow::heapify(std::vector<int>& arr, int n, int i) {
    int largest = i;
    int l = 2*i + 1, r = 2*i + 2;
    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void MainWindow::heapSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; --i) heapify(arr, n, i);
    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

void MainWindow::runSorts() {
    const size_t dataSize = 1000000;
    auto data1 = generateData(dataSize);
    auto data2 = data1;

    QElapsedTimer timer;

    timer.start();
    quickSort(data1, 0, static_cast<int>(data1.size() - 1));
    qint64 quickTime = timer.elapsed();

    timer.restart();
    heapSort(data2);
    qint64 heapTime = timer.elapsed();

    ui->resultLabel->setText(QString("QuickSort: %1 ms\nHeapSort: %2 ms").arg(quickTime).arg(heapTime));
}
