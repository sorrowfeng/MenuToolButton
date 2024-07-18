#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


Widget::Widget(QWidget *parent)
{
    ui->setupUi(this);

    // 创建一个 QList 对象来存储 QPixmap 类型的图像列表
    QList<QPixmap> pixmaps;
    // 创建一个 QStringList 对象来存储文本列表
    QStringList texts;
    
    // 将图像添加到 QPixmap 对象列表中,这里添加了两张图片
    pixmaps << QPixmap("://images/gif.png")
            << QPixmap("://images/csv.png")
            << QPixmap("://images/pdf.png");
    // 将文本添加到QStringList对象列表中，这里添加了两个文本项
    texts   << "Import GIF"
            << "Import CSV"
            << "Import PDF";
    // 获取用户界面中名为toolButton的指针，并设置其图标大小为42x42像素
    ui->toolButton->setIconSize(QSize(42, 42));
    // 获取用户界面中名为toolButton的指针，并设置其文字在图标的下方
    ui->toolButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    // 设置名为toolButton的按钮的图标列表，使用之前创建的 pixmaps 和 texts 列表
    ui->toolButton->setList(pixmaps, texts);
    // 设置名为toolButton的按钮的当前选中项索引为0，表示默认选中列表中的第一个项
    ui->toolButton->setCurrentItemIndex(0);
    // 设置名为toolButton的按钮的点击切换行为为true，即点击时点击时切换主按钮状态
    ui->toolButton->setClickToSwitch(true);
    // 设置名为toolButton的按钮的点击切换发射信号false，即点击时点击时发射点击信号
    ui->toolButton->setClickToSwitchWithEmit(true);
    // 设置名为toolButton的按钮的弹出区域可见性为true，即在显示弹出区域
    ui->toolButton->setPopAreaVisible(true);
    // 设置名为toolButton的按钮的弹出区域图片为://images/arrow.png，用于指示弹出区域
    ui->toolButton->setPopAreaPixmap(QPixmap("://images/arrow.png"));
    // 设置名为toolButton的按钮的弹出区域样式为MenuToolButton::PopAreaStyle::kToolButtonCornerRight，表示弹出区域位于按钮的右下角
    ui->toolButton->setPopAreaStyle(MenuToolButton::PopAreaStyle::kToolButtonCornerRight);

    // 建立信号槽连接，当菜单工具按钮被点击时，发射clicked信号，由当前类接收
    connect(ui->toolButton, &MenuToolButton::menuToolButton_clicked, this, [=](int i){
        // 当按钮被点击时，打印出传递的索引值i
        qDebug() << i << texts.at(i);
    });

}

Widget::~Widget()
{
    delete ui;
}
