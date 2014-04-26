#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QDragEnterEvent>
#include <QImageReader>
#include <QMimeData>

const QString DEFAULT_DIR_KEY = "DefaultDir";


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::MainWindow),
    sheet(2)
{
	ui->setupUi(this);
	ui->graphicsView->setAcceptDrops(false);
	setAcceptDrops(true);
    this->shortcutSwitch = new QShortcut(Qt::Key_Space, this);
    connect(this->shortcutSwitch, SIGNAL(activated()), this, SLOT(on_btSwitch_clicked()));

    this->palettePassive = this->paletteActive = ui->btOpen1->palette();
    this->paletteActive.setColor(QPalette::Button, Qt::green);
	ui->btOpen1->setAutoFillBackground(true);
	ui->btOpen2->setAutoFillBackground(true);

	this->setWindowState(Qt::WindowMaximized);

    QSet<QString> formatsSet;
    foreach (const QByteArray& format, QImageReader::supportedImageFormats())
    {
        formatsSet.insert(QString(format).toLower());
    }
    this->imageFormats = formatsSet.toList();
    this->imageFormats.sort();
    QStringList imageFormatsFilterList;
    foreach (const QString& format, this->imageFormats)
    {
        imageFormatsFilterList.append("*." + format);
    }
    this->imageFormatsFilter = QString("Изображения (%1)").arg(imageFormatsFilterList.join(" "));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		foreach (const QUrl &url, event->mimeData()->urls())
		{
            if (!this->urlToPath(url).isEmpty())
			{
				event->acceptProposedAction();
				return;
			}
		}
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
        for (int i = 0; i < event->mimeData()->urls().size() && i < this->sheet.size(); ++i)
		{
            QString path = this->urlToPath(event->mimeData()->urls().at(i));
			if (!path.isEmpty())
			{
				loadImage(path, i);
			}
		}
	}
}

void MainWindow::on_btOpen1_clicked()
{
	loadImage(QString::null, 0);
}

void MainWindow::on_btOpen2_clicked()
{
	loadImage(QString::null, 1);
}

void MainWindow::on_btSwitch_clicked()
{
	switchImage();
}

void MainWindow::on_slZoom_valueChanged(int value)
{
	qreal factor = static_cast<qreal>(value) / 100.0;

	ui->graphicsView->resetTransform();
	ui->graphicsView->scale(factor, factor);

	ui->edPercent->setText(QString::number(value) + QString("%"));
}

void MainWindow::on_slZoom_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
	ui->slZoom->setValue(100);
}

void MainWindow::on_graphicsView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
	on_btSwitch_clicked();
}


void MainWindow::loadImage(QString fileName, int pos)
{
	if (fileName.isEmpty())
	{
		QSettings settings;
		fileName = QFileDialog::getOpenFileName(this,
                                                QString("Выберите изображение ") + QString::number(pos + 1),
                                                settings.value(DEFAULT_DIR_KEY).toString(),
                                                this->imageFormatsFilter);

        if (fileName.isEmpty()) return;
		settings.setValue(DEFAULT_DIR_KEY, QFileInfo(fileName).absolutePath());
	}

    this->sheet[pos].clear();
    this->sheet[pos].scene = new QGraphicsScene();
    this->sheet[pos].pixmap = new QPixmap(fileName);

    int other = (pos + 1) % this->sheet.size();

	// Масштабируем
    if ( this->sheet[other].pixmap && this->sheet[other].scene &&
         ( this->sheet[other].pixmap->width() > this->sheet[pos].pixmap->width() ||
           this->sheet[other].pixmap->height() > this->sheet[pos].pixmap->height() ) )
	{
        this->sheet[pos].scene->addPixmap( this->sheet[pos].pixmap->scaled(
            this->sheet[other].pixmap->size(),
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation) );
        this->sheet[pos].scaled = true;

        if (this->sheet[other].scaled)
		{
            delete this->sheet[other].scene;
            this->sheet[other].scene = new QGraphicsScene();
            this->sheet[other].scene->addPixmap(*(this->sheet[other].pixmap));
            this->sheet[other].scaled = false;
		}
	}
	else
	{
        this->sheet[pos].scene->addPixmap(*(this->sheet[pos].pixmap));
        this->sheet[pos].scaled = false;

        if (this->sheet[other].pixmap && sheet[other].scene)
		{
            if (this->sheet[other].pixmap->width() < this->sheet[pos].pixmap->width() ||
                this->sheet[other].pixmap->height() < this->sheet[pos].pixmap->height())
			{
                delete this->sheet[other].scene;
                this->sheet[other].scene = new QGraphicsScene();
                this->sheet[other].scene->addPixmap( this->sheet[other].pixmap->scaled(
                    this->sheet[pos].pixmap->size(),
                    Qt::IgnoreAspectRatio,
                    Qt::SmoothTransformation) );
                this->sheet[other].scaled = true;
			}
            else if (this->sheet[other].scaled)
			{
                delete this->sheet[other].scene;
                this->sheet[other].scene = new QGraphicsScene();
                this->sheet[other].scene->addPixmap(*(this->sheet[other].pixmap));
                this->sheet[other].scaled = false;
			}
		}
	}

    this->sheet[pos].name = QFileInfo(fileName).fileName();
	switch (pos)
	{
	case 0:
        ui->btOpen1->setToolTip(this->sheet[pos].name);
		break;

	case 1:
        ui->btOpen2->setToolTip(this->sheet[pos].name);
		break;
	}

	// Центрирование изображения
	ui->slZoom->setValue(100);
	switchImage(pos);
    ui->graphicsView->centerOn(QPointF(this->sheet[pos].scene->width() / 2.0,
                                       this->sheet[pos].scene->height() / 2.0));

	// Выбор контрастного цвета
    /*QColor color(this->sheet[pos].pixmap->toImage().pixel(5, 5));
	if ((0.3 * color.redF() + 0.59 * color.greenF() + 0.11 * color.blueF()) > 0.25)
	{
        this->sheet[pos].namePalette.setColor(ui->lbName->foregroundRole(), Qt::black);
	}
	else
	{
        this->sheet[pos].namePalette.setColor(ui->lbName->foregroundRole(), Qt::white);
	}*/
}

void MainWindow::switchImage(int pos)
{
    static int lastPos = this->sheet.size() - 1;

    if (pos >= this->sheet.size())
	{
        lastPos = (lastPos + 1) % this->sheet.size();
	}
	else
	{
		lastPos = pos;
	}

    if (this->sheet[lastPos].scene)
	{
		QPointF center = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect()).boundingRect().center();
        ui->graphicsView->setScene(this->sheet[lastPos].scene);
		ui->graphicsView->centerOn(center);

        ui->lbName->setText(this->sheet[lastPos].name);
		switch (lastPos)
		{
		case 0:
            ui->btOpen1->setPalette(this->paletteActive);
            ui->btOpen2->setPalette(this->palettePassive);
			break;

		case 1:
            ui->btOpen1->setPalette(this->palettePassive);
            ui->btOpen2->setPalette(this->paletteActive);
			break;
		}
	}
}

QString MainWindow::urlToPath(const QUrl &url)
{
    QString path = url.toLocalFile();

    if (!path.isEmpty() && this->imageFormats.contains(QFileInfo(path).suffix(), Qt::CaseInsensitive))
    {
        return path;
    }

    return QString::null;
}
