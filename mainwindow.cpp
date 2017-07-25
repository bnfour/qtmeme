#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // pointer fields init
    originalImage = new QPixmap;
    copyToDrawOn = NULL;
    fontmetrics = NULL;
    // signal-slot magic
    connect(ui->aboutQtButton, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
    connect(ui->aboutButton, SIGNAL(clicked()), this, SLOT(about()));
    connect(ui->imageLoadButton, SIGNAL(clicked()), this, SLOT(openImage()));
    connect(ui->topEdit, SIGNAL(textChanged(QString)), this, SLOT(drawSlot()));
    connect(ui->bottomEdit, SIGNAL(textChanged(QString)), this, SLOT(drawSlot()));
    connect(ui->clipboardButton, SIGNAL(clicked()), this, SLOT(copyImageToClipboard()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveImage()));
    // initial size fixing
    resizeForm();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete originalImage;
    delete copyToDrawOn;
    delete fontmetrics;
}

void MainWindow::about()
{
    QMessageBox::about(this, "About meme generator", "<h2>Meme generator</h2><p>An app to create pictures with top and down captions in Impact font.</p><p>Written as an exercise in C++/Qt.</p>");
}

void MainWindow::openImage()
{
    // should be pretty straightforward
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("Images (*.png *.jpg *.bmp *.gif)");

    QString filename;

    if (dialog.exec())
    {
        filename = dialog.selectedFiles().first();
        originalImage->load(filename);
        copyToDrawOn = new QPixmap(*originalImage);
        resizeForm();
        ui->imgLabel->setPixmap(*originalImage);
    }
}

void MainWindow::drawSlot()
{
    // if we got something to draw
    if (copyToDrawOn)
    {
        // makeshift doublebuffering
        ui->imgLabel->setPixmap(*originalImage);
        // pointer magic
        delete copyToDrawOn;
        copyToDrawOn = new QPixmap(*originalImage);
        // getting maximum possible font size
        int font_size = configureFontAndGetSize(ui->topEdit->text(), ui->bottomEdit->text(), copyToDrawOn->width());
        // have no idea how to do it better
        int outline = int(font_size / outline_divisor);
        outline = outline > 1 ? outline : 1;
        // top line rendering
        QString top_text = ui->topEdit->text();
        QRect top(0, top_offset, copyToDrawOn->width(), int(3 * font_size / 2));
        drawOutlinedText(copyToDrawOn, top_text, top, font, outline);
        // bottom line rendering, DRY principle is sad about that
        QString bottom_text = ui->bottomEdit->text();
        QRect bottom(0, copyToDrawOn->height() - (bottom_offset + int(3 * font_size / 2)), copyToDrawOn->width(), int(3 * font_size / 2));
        drawOutlinedText(copyToDrawOn, bottom_text, bottom, font, outline);
        // switching "buffers" again
        ui->imgLabel->setPixmap(*copyToDrawOn);
    }
}

void MainWindow::resizeForm()
{
    // changes to 400×[menu height] on program start
    QSize picture_size = copyToDrawOn != NULL ? copyToDrawOn->size() : QSize(400, ui->frame->height());
    int picture_width = picture_size.width();
    int picture_height = picture_size.height();

    QSize menu_size = ui->frame->geometry().size();
    // +8 makes a neat right border
    int menu_width = menu_size.width() + 8;
    int menu_height = menu_size.height();

    int window_height = picture_height > menu_height ? picture_height : menu_height;
    int window_width = picture_width + menu_width;
    // resizing the window
    setFixedSize(window_width, window_height);
    // resizing the image which is actually a label
    ui->imgLabel->setGeometry(0, 0, picture_width, picture_height);
    // and moving the menu
    ui->frame->setGeometry(picture_width, 0, menu_width, menu_height);
    // enabling controls on first image load
    bool isFormEnabled = copyToDrawOn != NULL;
    ui->topEdit->setEnabled(isFormEnabled);
    ui->bottomEdit->setEnabled(isFormEnabled);
    ui->saveButton->setEnabled(isFormEnabled);
    ui->clipboardButton->setEnabled(isFormEnabled);

}

int MainWindow::configureFontAndGetSize(const QString &top_line, const QString &bottom_line, const int max_width)
{
    // tries font sizes from bigger to lower, stop on first (therefore maximum) size that fits
    for (int i = 0; i < font_sizes.length(); i++)
    {
        font = QFont("Impact", font_sizes[i]);
        fontmetrics = new QFontMetrics(font);
        int top_width = fontmetrics->width(top_line);
        int bottom_width = fontmetrics->width(bottom_line);
        if (top_width <= max_width && bottom_width <= max_width)
        {
            return font_sizes[i];
        }
        delete fontmetrics;
    }
    // line is too long, it's sad
    // so let's use everyone's favorite font to indicate that
    font = QFont("Comic Sans MS", 10);
    fontmetrics = new QFontMetrics(font);
    return 10;
}

void MainWindow::drawOutlinedText(QPixmap *pixmap, const QString &text, const QRect &rect, const QFont &font, const int outline_width)
{
    QPainter painter(pixmap);

    painter.setFont(font);
    // black and white colors are hardcoded
    painter.setPen(Qt::black);

    // generates "outline" as inspired by https://www.youtube.com/watch?v=ft_RGEThDVA
    for (int i = -outline_width; i <= outline_width; i++)
    {
        for (int j = -outline_width; j <= outline_width; j++)
        {
            if (i || j)
            {
                QRect moved_rect = rect.translated(i, j);
                painter.drawText(moved_rect, Qt::AlignVCenter | Qt::AlignHCenter, text);
            }
        }
    }
    // main text
    painter.setPen(Qt::white);
    painter.drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, text);
}

void MainWindow::copyImageToClipboard()
{
    // it's really simple in Qt
    QApplication::clipboard()->setPixmap(*copyToDrawOn);
}

void MainWindow::saveImage()
{
    // also straightforward
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter("PNG images (*.png)");

    QString filename;
    if (dialog.exec())
    {
        filename = dialog.selectedFiles().first();
        copyToDrawOn->save(filename, "PNG");
    }
}
