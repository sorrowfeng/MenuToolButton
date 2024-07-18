#ifndef MENUTOOLBUTTON_H
#define MENUTOOLBUTTON_H

#include <QWidget>
#include <QToolButton>
#include <QMenu>
#include <QPixmap>
#include <QString>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QEvent>
#include <QFont>


class MenuToolButtonItem : public QWidget
{
    Q_OBJECT
public:
    explicit MenuToolButtonItem(QWidget *parent = nullptr);
    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);
    QString text() const;
    void setText(const QString &text);
    QFont font() const;
    void setFont(const QFont &font);
    Qt::ToolButtonStyle displayMode() const;
    void setDisplayMode(Qt::ToolButtonStyle mode);


    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

private:
    void paintIconOnly(QPainter *painter, const QRect &rect);
    void paintTextOnly(QPainter *painter, const QRect &rect);
    void paintTextBesideIcon(QPainter *painter, const QRect &rect);
    void paintTextUnderIcon(QPainter *painter, const QRect &rect);


signals:
    void clicked();

private:
    QPixmap m_pixmap;
    QString m_text;
    QFont m_font;
    Qt::ToolButtonStyle m_displayMode;

    int m_margin;
    int m_interval;
    bool m_isPressed;
    bool m_isHovered;
};




class MenuToolButton : public QToolButton
{
    Q_OBJECT
public:
    /**
     * @brief 定义一个名为 PopAreaStyle 的枚举类，用于表示菜单按钮弹出区域的样式
     */
    enum class PopAreaStyle {
        /**
         * @brief 菜单按钮弹出区域在按钮的中心右边
         */
        kToolButtonCenterRight,
        /**
         * @brief 菜单按钮弹出区域在按钮的中心左边
         */
        kToolButtonCenterLeft,
        /**
         * @brief 菜单按钮弹出区域在按钮的右下角
         */
        kToolButtonCornerRight,
        /**
         * @brief 菜单按钮弹出区域在按钮的左下角
         */
        kToolButtonCornerLeft
    };


    explicit MenuToolButton(QWidget *parent = nullptr);

    /**
     * @brief 设置弹出项目列表
     * @param pixmaps 图片列表
     * @param list 图片列表对应的字符串列表
     */
    void setList(const QList<QPixmap> &pixmaps, const QStringList &list);

    /**
     * @brief 设置当前项索引，即主按钮当前索引
     * @param index 要设置的当前项索引
     */
    void setCurrentItemIndex(int index);

    /**
     * @brief 设置点击时是否切换
     * @param enable 一个布尔值，指示点击时是否切换主按钮状态
     */
    void setClickToSwitch(bool enable);

    /**
     * @brief 设置点击切换时是否发射点击信号
     *
     * @param enable 一个布尔值，指示是否启用
     */
    void setClickToSwitchWithEmit(bool enable);

    /**
     * @brief 设置弹出区域是否可见
     * @param visible 一个布尔值，指示是否显示弹出区域
     */
    void setPopAreaVisible(bool visible);

    /**
     * @brief 设置弹出区域的图片
     * @param pixmap 要设置的弹出区域图片
     * @note 设置弹出区域下拉箭头的图片
     */
    void setPopAreaPixmap(const QPixmap& pixmap);

    /**
     * @brief 设置弹出区域的样式
     * @param style 要设置的弹出区域样式
     * @note 设置弹出区域样式，即下拉箭头所在区域的位置
     */
    void setPopAreaStyle(const PopAreaStyle& style);


signals:
    /**
     * @brief 点击按钮或下拉项目时发出的信号
     * @param 表示被点击的按钮索引
     */
    void menuToolButton_clicked(int);



private:
    MenuToolButtonItem* menuAddAction(const QPixmap& pixmap, const QString& text);
    void showMenu();
    QRect getPopAreaRect();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;


private:
    QMenu* m_menu = nullptr;
    QList<MenuToolButtonItem*> m_menu_items;
    int m_current_index = -1;
    MenuToolButtonItem* m_current_item = nullptr;
    bool m_click_to_switch = false;
    bool m_click_to_switch_with_emit = true;
    bool m_show_pop_area = false;
    QPixmap m_pop_area_arrow_pix;
    PopAreaStyle m_pop_area_style = PopAreaStyle::kToolButtonCenterRight;
};

#endif // MENUTOOLBUTTON_H
