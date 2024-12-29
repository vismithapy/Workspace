#include "mainwindow.h"
#include <QBrush>
#include <QColorDialog>
#include <QComboBox>
#include <QDomDocument>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QRandomGenerator>
#include "customitem.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentRect(nullptr)
    , currentEllipse(nullptr)
    , currentPolygonItem(nullptr)
    , selectedShape("Rectangle")
    ,                       // Default shape
    selectedColor(Qt::blue) // Default color

{
    ui->setupUi(this);

    setWindowTitle("Workplace Planner");
    resize(1200, 800);

    // Initialize the scene
    scene = new QGraphicsScene(this);
    // Set background image for the workplace area
    QPixmap background("D:/QT Application/App/WorkPlacePlanner/Image/assets/floor.jpg");
    background = background.scaled(2000,
                                   2000,
                                   Qt::KeepAspectRatioByExpanding,
                                   Qt::SmoothTransformation);
    scene->setBackgroundBrush(QBrush(background));

    ui->graphicsViewCanvas->setScene(scene);
    ui->graphicsViewCanvas->setFocus();
    ui->graphicsViewCanvas->setInteractive(true);

    setupUI();
    drawGrid();
    setupMenuBar();
    drawSections();

    currentScaleFactor = 1.0;
}

void MainWindow::deleteSelectedItem()
{
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();
    for (QGraphicsItem *item : selectedItems) {
        scene->removeItem(item);
        delete item; // Delete the selected item
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetCurrentShape()
{
    currentRect = nullptr;
    currentEllipse = nullptr;
    currentPolygon.clear();
    currentPolygonItem = nullptr;
}

void MainWindow::createShape()
{
    // Add logic to create a shape dynamically
    QMessageBox::information(this, "Create Shape", "Shape creation logic goes here.");
}
void MainWindow::selectRectangle()
{
    selectedShape = "Rectangle";
}

void MainWindow::selectCircle()
{
    selectedShape = "Circle";
}

void MainWindow::selectTriangle()
{
    selectedShape = "Triangle";
}

void MainWindow::addRectangleToCanvas() {
    CustomItem *rect = new CustomItem("Rectangle", "");
    rect->setShapeType(CustomItem::ShapeType::Rectangle);
    rect->setShapeColor(QColor("#4CAF50")); // Set a pleasing default color

    QPixmap pixmap(150, 100);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setBrush(rect->getShapeColor());
    painter.drawRect(0, 0, 150, 100);
    painter.end();

    rect->setPixmap(pixmap);
    rect->setPos(100, 100);
    scene->addItem(rect);
}

void MainWindow::addCircleToCanvas() {
    CustomItem *circle = new CustomItem("Circle", "");
    circle->setShapeType(CustomItem::ShapeType::Circle);
    circle->setShapeColor(QColor("#4CAF50")); // Set a pleasing default color

    QPixmap pixmap(100, 100);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setBrush(circle->getShapeColor());
    painter.drawEllipse(0, 0, 100, 100);
    painter.end();

    circle->setPixmap(pixmap);
    circle->setPos(100, 100);
    scene->addItem(circle);
}

void MainWindow::addTriangleToCanvas() {
    CustomItem *triangle = new CustomItem("Triangle", "");
    triangle->setShapeType(CustomItem::ShapeType::Triangle);
    triangle->setShapeColor(QColor("#4CAF50")); // Set a pleasing default color

    QPixmap pixmap(150, 150);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setBrush(triangle->getShapeColor());
    QPolygonF polygon;
    polygon << QPointF(75, 0) << QPointF(150, 150) << QPointF(0, 150);
    painter.drawPolygon(polygon);
    painter.end();

    triangle->setPixmap(pixmap);
    triangle->setPos(100, 100);
    scene->addItem(triangle);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    startPoint = ui->graphicsViewCanvas->mapToScene(event->pos());

    if (selectedShape == "Rectangle") {
        currentRect = scene->addRect(QRectF(startPoint, QSizeF()),
                                     QPen(selectedColor),
                                     QBrush(selectedColor, Qt::SolidPattern));
    } else if (selectedShape == "Circle") {
        currentEllipse = scene->addEllipse(QRectF(startPoint, QSizeF()),
                                           QPen(selectedColor),
                                           QBrush(selectedColor, Qt::SolidPattern));
    } else if (selectedShape == "Triangle") {
        currentPolygon.clear();
        currentPolygon << startPoint;
        currentPolygonItem = scene->addPolygon(currentPolygon,
                                               QPen(selectedColor),
                                               QBrush(selectedColor, Qt::SolidPattern));
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPointF currentPoint = ui->graphicsViewCanvas->mapToScene(event->pos());

        if (currentRect) {
            QRectF newRect(startPoint, currentPoint);
            currentRect->setRect(newRect.normalized());
        } else if (currentEllipse) {
            QRectF newEllipse(startPoint, currentPoint);
            currentEllipse->setRect(newEllipse.normalized());
        } else if (currentPolygonItem) {
            QPointF thirdPoint((startPoint.x() + currentPoint.x()) / 2, currentPoint.y());
            currentPolygon.clear();
            currentPolygon << startPoint << currentPoint << thirdPoint;
            currentPolygonItem->setPolygon(currentPolygon);
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        currentRect = nullptr;
        currentEllipse = nullptr;
        currentPolygonItem = nullptr;
    }
}
void MainWindow::setColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Select Color");
    if (color.isValid()) {
        selectedColor = color;
    }
}
void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);

    // File Menu
    QMenu *fileMenu = menuBar->addMenu("File");
    QAction *saveAction = new QAction("Save", this);
    QAction *saveAsAction = new QAction("Save As", this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    // Edit Menu
    QMenu *editMenu = menuBar->addMenu("Edit");
    QAction *textFieldAction = new QAction("Add Text Field", this);
    connect(textFieldAction, &QAction::triggered, this, &MainWindow::addTextField);
    editMenu->addAction(textFieldAction);

    // Create Plan Menu
    QMenu *createMenu = menuBar->addMenu("Create Plan");
    QAction *circleAction = new QAction("Circle", this);
    QAction *rectangleAction = new QAction("Rectangle", this);
    QAction *triangleAction = new QAction("Triangle", this);
    QAction *colorPickerAction = new QAction("Select Color", this);
    //QAction *customAction = new QAction("Custom (Pen Tool)", this);
    connect(rectangleAction, &QAction::triggered, this, &MainWindow::addRectangleToCanvas);
    connect(circleAction, &QAction::triggered, this, &MainWindow::addCircleToCanvas);
    connect(triangleAction, &QAction::triggered, this, &MainWindow::addTriangleToCanvas);
    connect(colorPickerAction, &QAction::triggered, this, &MainWindow::setColor);
    //connect(customAction, &QAction::triggered, this, &MainWindow::enableCustomDrawing);
    createMenu->addAction(rectangleAction);
    createMenu->addAction(circleAction);
    createMenu->addAction(triangleAction);
    createMenu->addAction(colorPickerAction);
    //createMenu->addAction(customAction);

    // Color Picker
    // QAction *colorPickerAction = new QAction("Select Color", this);
    // connect(colorPickerAction, &QAction::triggered, this, &MainWindow::setColor);
    // createMenu->addAction(colorPickerAction);

    setMenuBar(menuBar);
}

// Initialize the UI
void MainWindow::setupUI()
{
    // Add items to the list widget
    QStringList items = {"Chair", "Table", "Work Zone", "Office Zone", "Storage Zone"};
    for (const QString &item : items) {
        ui->listWidgetItems->addItem(item);
    }

    // Product dropdown menu
    productComboBox = new QComboBox(this);
    productComboBox->addItems({"Pot", "Lamp", "Plant", "Shelf"});
    productComboBox->setFixedSize(200, 30);
    productComboBox->move(10, 200);

    QPushButton *previewButton = new QPushButton("Preview", this);
    previewButton->setFixedSize(200, 30);
    previewButton->move(10, 360);
    connect(previewButton, &QPushButton::clicked, this, &MainWindow::showPreview);
    // Add Product button
    QPushButton *addProductButton = new QPushButton("Add Product", this);
    addProductButton->setFixedSize(200, 30);
    addProductButton->move(10, 240);

    // Zoom In button
    QPushButton *zoomInButton = new QPushButton("Zoom In", this);
    zoomInButton->setFixedSize(200, 30);
    zoomInButton->move(10, 280);

    // Zoom Out button
    QPushButton *zoomOutButton = new QPushButton("Zoom Out", this);
    zoomOutButton->setFixedSize(200, 30);
    zoomOutButton->move(10, 320);

    // Connect signals to slots
    connect(ui->listWidgetItems, &QListWidget::itemClicked, this, &MainWindow::addItemToScene);
    connect(ui->buttonUploadAssets, &QPushButton::clicked, this, &MainWindow::uploadAssets);
    connect(addProductButton, &QPushButton::clicked, this, &MainWindow::addProductToScene);
    connect(zoomInButton, &QPushButton::clicked, this, &MainWindow::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &MainWindow::zoomOut);

    // Set the initial zoom scale
    currentScaleFactor = 1.0;
}

void MainWindow::saveFile()
{
    exportToXML();
}
void MainWindow::saveFileAs()
{
    exportToXML();
}

void MainWindow::showPreview()
{
    // Create a dialog for the preview
    QDialog *previewDialog = new QDialog(this);
    previewDialog->setWindowTitle("Workplace Preview");
    previewDialog->resize(800, 600);

    // Create a graphics view for the preview
    QGraphicsView *previewView = new QGraphicsView(previewDialog);
    previewView->setGeometry(0, 0, 800, 600);

    // Create a temporary QGraphicsScene for rendering the preview
    QGraphicsScene *previewScene = new QGraphicsScene();

    // Render the existing scene content to an image
    QRectF sceneRect = scene->sceneRect();
    QImage previewImage(sceneRect.size().toSize(), QImage::Format_ARGB32);
    previewImage.fill(Qt::white); // Optional: set background color to white

    QPainter painter(&previewImage);
    scene->render(&painter);

    // Add the rendered image to the preview scene
    previewScene->addPixmap(QPixmap::fromImage(previewImage));

    // Set the preview scene in the graphics view
    previewView->setScene(previewScene);

    // Show the preview dialog
    previewDialog->exec();

    // Clean up
    delete previewScene;
}

void MainWindow::uploadAssets()
{
    QString initialPath
        = "D:/QT Application/App/WorkPlacePlanner/Image/assets"; // Initial directory path

    // Open a file dialog to let the user select images
    QStringList fileNames
        = QFileDialog::getOpenFileNames(this,
                                        "Select Images to Upload",
                                        initialPath,
                                        "Images (*.png *.jpg *.jpeg)" // Filter for image file types
        );

    if (fileNames.isEmpty()) {
        QMessageBox::information(this, "No Selection", "No images were selected.");
        return; // Exit if no files are selected
    }

    // Add the selected images to the scene
    for (const QString &filePath : fileNames) {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();

        // Create a new CustomItem and set a random position
        CustomItem *customItem = new CustomItem(fileName, filePath);
        customItem->setPos(QRandomGenerator::global()->bounded(500),
                           QRandomGenerator::global()->bounded(500)); // Set random position

        scene->addItem(customItem); // Add the item to the scene
    }

    QMessageBox::information(this,
                             "Success",
                             "Selected images have been uploaded to the workplace area.");
}

void MainWindow::addTextField()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Add Text", "Enter text:", QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        QGraphicsTextItem *textItem = scene->addText(text);
        textItem->setPos(100, 100);
        textItem->setFlag(QGraphicsItem::ItemIsMovable);
    }
}

void MainWindow::drawCircle()
{
    scene->addEllipse(100, 100, 200, 200, QPen(Qt::blue), QBrush(Qt::transparent));
}

void MainWindow::drawRectangle()
{
    scene->addRect(100, 100, 200, 150, QPen(Qt::green), QBrush(Qt::transparent));
}

void MainWindow::drawTriangle()
{
    QPolygonF triangle;
    triangle << QPointF(150, 100) << QPointF(250, 300) << QPointF(50, 300);
    scene->addPolygon(triangle, QPen(Qt::red), QBrush(Qt::transparent));
}

// Custom Pen Tool
void MainWindow::enableCustomDrawing()
{
    QMessageBox::information(this, "Custom Drawing", "Custom drawing with pen enabled!");
    customDrawingEnabled = true;
}

void MainWindow::zoomIn()
{
    if (currentScaleFactor < 3.0) { // Maximum zoom-in limit (3x)
        currentScaleFactor *= 1.2;  // Increase scale by 20%
        ui->graphicsViewCanvas->scale(1.2, 1.2);
    }
}

void MainWindow::zoomOut()
{
    if (currentScaleFactor > 0.5) { // Minimum zoom-out limit (0.5x)
        currentScaleFactor /= 1.2;  // Decrease scale by 20%
        ui->graphicsViewCanvas->scale(1 / 1.2, 1 / 1.2);
    }
}

// Draw grid lines for alignment
void MainWindow::drawGrid()
{
    const int gridSize = 50;      // Grid cell size
    const int sceneWidth = 2000;  // New width
    const int sceneHeight = 2000; // New height
    QPen gridPen(QColor(200, 200, 200, 50), 1, Qt::DotLine);

    // Draw vertical grid lines
    for (int x = 0; x < sceneWidth; x += gridSize)
        scene->addLine(x, 0, x, sceneHeight, gridPen)->setZValue(-10);

    // Draw horizontal grid lines
    for (int y = 0; y < sceneHeight; y += gridSize)
        scene->addLine(0, y, sceneWidth, y, gridPen)->setZValue(-10);

    // Set the scene rectangle to the new size
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
}

//Draw bold section lines and add labels
void MainWindow::drawSections()
{
    const int sceneWidth = 1200;  // Total scene width
    const int sceneHeight = 1200; // Total scene height

    QPen borderPen(QColor(0, 0, 0), 2); // Black border lines

    // Resize scene to fit the full layout
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    // Draw workplace border (only the outer border)
    scene->addRect(0, 0, sceneWidth, sceneHeight, borderPen); // Horizontal line for Table Zone
}

void MainWindow::addProductToScene()
{
    QString selectedProduct = productComboBox->currentText();
    CustomItem *customItem = nullptr;

    // Assign images for the additional products
    if (selectedProduct == "Pot") {
        customItem = new CustomItem("Pot", ":/images/Image/pot");
    } else if (selectedProduct == "Lamp") {
        customItem = new CustomItem("Lamp", ":/images/Image/lamp");
    } else if (selectedProduct == "Plant") {
        customItem = new CustomItem("Plant", ":/images/Image/plant");
    } else if (selectedProduct == "Shelf") {
        customItem = new CustomItem("Shelf", ":/images/Image/shelf");
    }

    // Add the product to the scene without zone checks
    if (customItem) {
        customItem->setZValue(0);
        scene->addItem(customItem);
        customItem->setPos(100, 100); // Place at a default position
        customItem->setSelected(true);
    }
}

// Add an item to the workspace
void MainWindow::addItemToScene(QListWidgetItem *item)
{
    CustomItem *customItem = nullptr;

    // Determine the product and set its image
    if (item->text() == "Chair") {
        customItem = new CustomItem("Chair", ":/images/Image/chair1");
        customItem->setPos(-25, -25);
    } else if (item->text() == "Table") {
        customItem = new CustomItem("Table", ":/images/Image/table1");
        customItem->setPos(-25, -25);
    } else if (item->text() == "Work Zone") {
        customItem = new CustomItem("Work Zone", ":/images/Image/work zone1");
        customItem->setPos(-25, -25);
    } else if (item->text() == "Office Zone") {
        customItem = new CustomItem("Office Zone", ":/images/Image/office zone1");
        customItem->setPos(-25, -25);
    } else if (item->text() == "Storage Zone") {
        customItem = new CustomItem("Storage Zone", ":/images/Image/storage zone1");
        customItem->setPos(-25, -25);
    }

    // Add the product to the scene
    if (customItem) {
        customItem->setZValue(0);
        scene->addItem(customItem);
        customItem->setSelected(true);
    }
}

// Export the workspace to an XML file
void MainWindow::exportToXML()
{
    QDomDocument doc;
    QDomElement root = doc.createElement("workspace");
    doc.appendChild(root);

    for (QGraphicsItem *item : scene->items()) {
        CustomItem *customItem = dynamic_cast<CustomItem *>(item);
        if (customItem) {
            QDomElement element = doc.createElement("Object");
            element.setAttribute("name", customItem->getName());
            element.setAttribute("x", customItem->x());
            element.setAttribute("y", customItem->y());
            element.setAttribute("width", customItem->pixmap().width());
            element.setAttribute("height", customItem->pixmap().height());
            element.setAttribute("rotation", customItem->rotation());
            element.setAttribute("zValue", customItem->zValue());
            root.appendChild(element);
        }
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save As XML", "", "XML Files (*.xml)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            doc.save(stream, 4);
            file.close();
            QMessageBox::information(this, "Success", "Plan saved as XML successfully!");
        }
    }
}

// Import a workspace from an XML file
void MainWindow::importFromXML()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import Plan", "", "XML Files (*.xml)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Failed to open XML file.");
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        QMessageBox::critical(this, "Error", "Failed to parse XML file.");
        file.close();
        return;
    }
    file.close();

    scene->clear();
    drawGrid();
    drawSections();

    QDomNodeList objects = doc.documentElement().elementsByTagName("Object");

    for (int i = 0; i < objects.count(); ++i) {
        QDomElement element = objects.at(i).toElement();

        QString name = element.attribute("name");
        qreal x = element.attribute("x").toDouble();
        qreal y = element.attribute("y").toDouble();
        qreal width = element.attribute("width").toDouble();
        qreal height = element.attribute("height").toDouble();
        qreal rotation = element.attribute("rotation").toDouble();
        qreal zValue = element.attribute("zValue").toDouble();

        QString imagePath = ":/images/Image/chair"; // Default
        if (name == "Chair")
            imagePath = ":/images/Image/chair1";
        else if (name == "Table")
            imagePath = ":/images/Image/table1";
        else if (name == "Work Zone")
            imagePath = ":/images/Image/work zone1";
        else if (name == "Office Zone")
            imagePath = ":/images/Image/office zone1";
        else if (name == "Storage Zone")
            imagePath = ":/images/Image/storage zone1";

        CustomItem *item = new CustomItem(name, imagePath);
        item->setPos(x, y);
        item->setRotation(rotation);
        item->setZValue(zValue);
        item->setPixmap(
            item->pixmap().scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        scene->addItem(item);
    }

    QMessageBox::information(this, "Success", "Plan imported successfully!");
}
