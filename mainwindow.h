#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QClipboard>

namespace Ui {
class MainWindow;
}
// should be in decreasing order
const QVector<int> font_sizes = {72, 56, 48, 36, 32, 28, 24, 18, 16, 12, 10};
// offsets of text rects from top and bottom of picture
const int top_offset = 12;
const int bottom_offset = 12;
// outline width = size / outline_divisor,
// so 3 on 72, 2 on 56 and 48, 1 on every other size
// there is always at least one pixel of outline
const int outline_divisor = 24;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void about();
    void openImage();
    void drawSlot();
    void copyImageToClipboard();
    void saveImage();

private:
    Ui::MainWindow *ui;
    QPixmap *originalImage;
    QPixmap *copyToDrawOn;
    QFont font;
    QFontMetrics *fontmetrics;
    // members above, methods below this line
    void resizeForm();
    void drawOutlinedText(QPixmap *pixmap, const QString& text, const QRect &rect, const QFont &font, const int outline_width);
    int configureFontAndGetSize(const QString& top_line, const QString& bottom_line, const int max_width);
};

#endif // MAINWINDOW_H
