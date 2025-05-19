#include "MenuToolButton.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QFontMetrics>
#include <QIcon>
#include <QPainter>
#include <QStyle>
#include <QWidgetAction>
#include <QtMath>

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#define FM_WIDTH(fm, str) (fm).horizontalAdvance(str)
#else
#define FM_WIDTH(fm, str) (fm).width(str)
#endif

MenuToolButtonItem::MenuToolButtonItem(QWidget *parent) : QWidget(parent) {
  m_margin = 2;
  m_interval = 2;
  m_isPressed = false;
  m_isHovered = false;
}

QPixmap MenuToolButtonItem::pixmap() const { return m_pixmap; }

void MenuToolButtonItem::setPixmap(const QPixmap &pixmap) {
  m_pixmap = pixmap;
  update();
}

QString MenuToolButtonItem::text() const { return m_text; }

void MenuToolButtonItem::setText(const QString &text) {
  m_text = text;
  update();
}

QFont MenuToolButtonItem::font() const { return m_font; }

void MenuToolButtonItem::setFont(const QFont &font) {
  m_font = font;
  update();
}

Qt::ToolButtonStyle MenuToolButtonItem::displayMode() const {
  return m_displayMode;
}

void MenuToolButtonItem::setDisplayMode(Qt::ToolButtonStyle mode) {
  m_displayMode = mode;
  update();
}

void MenuToolButtonItem::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  QRect rect = this->rect();

  painter.save();
  // 根据状态设置背景颜色
  if (m_isPressed) {
    painter.fillRect(rect, QColor("#cce4f7"));  // 按下时的颜色
  } else if (m_isHovered) {
    painter.fillRect(rect, QColor("#e0eef9"));  // 悬停时的颜色
  } else {
    painter.fillRect(rect, QColor("#fdfdfd"));  // 默认颜色
  }

  //    // 绘制边框
  //    painter.setPen(QColor("#0078d4"));
  painter.setPen(Qt::gray);
  painter.drawRect(rect.adjusted(0, 0, -1, -1));

  painter.restore();

  // rect间隔
  rect = rect.marginsRemoved(QMargins(m_margin, m_margin, m_margin, m_margin));

  switch (m_displayMode) {
    case Qt::ToolButtonStyle::ToolButtonIconOnly:
      paintIconOnly(&painter, rect);
      break;
    case Qt::ToolButtonStyle::ToolButtonTextOnly:
      paintTextOnly(&painter, rect);
      break;
    case Qt::ToolButtonStyle::ToolButtonTextBesideIcon:
      paintTextBesideIcon(&painter, rect);
      break;
    case Qt::ToolButtonStyle::ToolButtonTextUnderIcon:
      paintTextUnderIcon(&painter, rect);
      break;
  }
}

void MenuToolButtonItem::paintIconOnly(QPainter *painter, const QRect &rect) {
  if (!m_pixmap.isNull()) {
    painter->setFont(m_font);
    int pixmapWidth = rect.width();
    int pixmapHeight = rect.height();
    QPixmap draw_pixmap =
        m_pixmap.scaled(pixmapWidth, pixmapHeight, Qt::KeepAspectRatio);
    painter->drawPixmap(rect.center() - draw_pixmap.rect().center(),
                        draw_pixmap);
  }
}

void MenuToolButtonItem::paintTextOnly(QPainter *painter, const QRect &rect) {
  if (!m_text.isEmpty()) {
    painter->setFont(m_font);
    painter->drawText(rect, Qt::AlignCenter, m_text);
  }
}

void MenuToolButtonItem::paintTextBesideIcon(QPainter *painter,
                                             const QRect &rect) {
  if (!m_pixmap.isNull() && !m_text.isEmpty()) {
    painter->setFont(m_font);
    int textWidth = FM_WIDTH(QFontMetrics(m_font), m_text) + m_interval;
    int textHeight = QFontMetrics(m_font).height();
    int pixmapWidth = rect.width() - textWidth;
    int pixmapHeight = rect.height();

    int minPixLen = qMin(pixmapWidth, pixmapHeight);
    pixmapWidth = minPixLen;
    pixmapHeight = minPixLen;

    QRect iconRect = rect.adjusted(0, 0, pixmapWidth - rect.width(), 0);
    QRect textRect = rect.adjusted(pixmapWidth, 0, 0, 0);

    QPixmap draw_pixmap =
        m_pixmap.scaled(pixmapWidth, pixmapHeight, Qt::KeepAspectRatio);
    painter->drawPixmap(iconRect.center() - draw_pixmap.rect().center(),
                        draw_pixmap);
    painter->drawText(textRect, Qt::AlignCenter, m_text);
  }
}

void MenuToolButtonItem::paintTextUnderIcon(QPainter *painter,
                                            const QRect &rect) {
  if (!m_pixmap.isNull() && !m_text.isEmpty()) {
    painter->setFont(m_font);
    int textWidth = FM_WIDTH(QFontMetrics(m_font), m_text);
    int textHeight = QFontMetrics(m_font).height() + m_interval;
    int pixmapWidth = rect.width();
    int pixmapHeight = rect.height() - textHeight;

    QRect iconRect = rect.adjusted(0, 0, 0, -textHeight);
    QRect textRect = rect.adjusted(0, pixmapHeight, 0, 0);

    QPixmap draw_pixmap =
        m_pixmap.scaled(pixmapWidth, pixmapHeight, Qt::KeepAspectRatio);
    painter->drawPixmap(iconRect.center() - draw_pixmap.rect().center(),
                        draw_pixmap);
    painter->drawText(textRect, Qt::AlignCenter, m_text);
  }
}

void MenuToolButtonItem::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    m_isPressed = true;
    update();
  }
}

void MenuToolButtonItem::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    m_isPressed = false;
    emit clicked();
    update();
  }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void MenuToolButtonItem::enterEvent(QEnterEvent *event) {
#else
void MenuToolButtonItem::enterEvent(QEvent *event) {
#endif
  Q_UNUSED(event);
  m_isHovered = true;
  update();
}

void MenuToolButtonItem::leaveEvent(QEvent *event) {
  Q_UNUSED(event);
  m_isHovered = false;
  update();
}

MenuToolButton::MenuToolButton(QWidget *parent)
    : QToolButton(parent), m_menu(new QMenu(this)) {
  // 构造函数的实现
  this->setCheckable(false);
}

void MenuToolButton::setList(const QList<QPixmap> &pixmaps,
                             const QStringList &list) {
  m_menu->clear();
  m_menu_items.clear();
  if (list.length() != pixmaps.length()) return;
  for (int i = 0; i < list.length(); ++i) {
    QString text = list.at(i);
    QPixmap pixmap = pixmaps.at(i);
    MenuToolButtonItem *menu_item = menuAddAction(pixmap, text);
    connect(menu_item, &MenuToolButtonItem::clicked, this, [=]() {
      if (m_click_to_switch && !m_click_to_switch_with_emit) {
        // 此情况不发信号，其他情况都发信号
      } else {
        emit menuToolButton_clicked(i);
      }
      m_menu->close();

      this->setFocus();
      this->setChecked(false);

      if (m_click_to_switch) {
        setCurrentItemIndex(i);
      }
    });
  }
  setCurrentItemIndex(0);
}

void MenuToolButton::setCurrentItemIndex(int index) {
  if (m_current_index == index) return;

  m_current_index = index;
  if (index >= m_menu_items.size()) return;

  MenuToolButtonItem *item = m_menu_items.at(index);
  m_current_item = item;

  QPixmap pixmap = item->pixmap();
  QString text = item->text();

  setIcon(QIcon(pixmap));
  setText(text);

  update();
}

void MenuToolButton::setClickToSwitch(bool enable) {
  m_click_to_switch = enable;
}

void MenuToolButton::setClickToSwitchWithEmit(bool enable) {
  m_click_to_switch_with_emit = enable;
}

void MenuToolButton::setPopAreaVisible(bool visible) {
  m_show_pop_area = visible;
}

void MenuToolButton::setPopAreaPixmap(const QPixmap &pixmap) {
  m_pop_area_arrow_pix = pixmap;
}

void MenuToolButton::setPopAreaStyle(
    const MenuToolButton::PopAreaStyle &style) {
  m_pop_area_style = style;
}

MenuToolButtonItem *MenuToolButton::menuAddAction(const QPixmap &pixmap,
                                                  const QString &text) {
  MenuToolButtonItem *widget = new MenuToolButtonItem;
  widget->setPixmap(pixmap);
  widget->setText(text);
  widget->setFont(this->font());
  widget->setDisplayMode(this->toolButtonStyle());
  widget->setFixedSize(width(), height());

  QWidgetAction *action = new QWidgetAction(m_menu);
  action->setDefaultWidget(widget);

  m_menu_items.append(widget);
  m_menu->addAction(action);

  return widget;
}

void MenuToolButton::showMenu() {
  // 设置菜单项的宽高与按钮高度相同
  int menu_w = width();
  int menu_h = 0;
  foreach (MenuToolButtonItem *widget, m_menu_items) {
    widget->setFixedSize(width(), height());
    widget->setDisplayMode(this->toolButtonStyle());
    menu_h += height();
  }
  m_menu->setFixedWidth(menu_w);
  m_menu->setFixedHeight(menu_h);

  // 计算菜单显示的位置
  QPoint globalPos = mapToGlobal(QPoint(0, height()));
  m_menu->exec(globalPos);

  this->setFocus();
  this->setChecked(false);
}

void MenuToolButton::contextMenuEvent(QContextMenuEvent *event) { showMenu(); }

void MenuToolButton::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (m_show_pop_area) {
      QRect poprect = getPopAreaRect();
      if (poprect.contains(event->pos())) {
        showMenu();
        QToolButton::mouseReleaseEvent(event);
        return;
      }
    }

    emit menuToolButton_clicked(m_current_index);
  }
  QToolButton::mouseReleaseEvent(event);
}

void MenuToolButton::paintEvent(QPaintEvent *event) {
  QToolButton::paintEvent(event);  // 调用基类的 paintEvent 方法

  if (m_show_pop_area) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    QRect poprect = getPopAreaRect();
    if (m_pop_area_style == PopAreaStyle::kToolButtonCenterRight) {
      // 画竖线
      painter.setPen(QPen(Qt::gray, 1));  // 设置画笔颜色和宽度
      painter.drawLine(poprect.left(), rect.top() + 5, poprect.left(),
                       rect.bottom() - 5);
    } else if (m_pop_area_style == PopAreaStyle::kToolButtonCenterLeft) {
      // 画竖线
      painter.setPen(QPen(Qt::gray, 1));  // 设置画笔颜色和宽度
      painter.drawLine(poprect.right(), rect.top() + 5, poprect.right(),
                       rect.bottom() - 5);
    } else if (m_pop_area_style == PopAreaStyle::kToolButtonCornerRight) {
    } else if (m_pop_area_style == PopAreaStyle::kToolButtonCornerLeft) {
    }

    // 画箭头
    if (m_pop_area_arrow_pix.isNull()) {
      painter.setPen(QPen(Qt::black, 2));  // 设置画笔颜色和宽度
      int W1 = poprect.left();
      int W2 = poprect.width();
      int L1 = W2 * 1 / 3;
      int H1 = poprect.center().y();
      QPoint p1(W1 + L1, H1);
      QPoint p2(W1 + L1 * 2, H1);
      QPoint p3(W1 + W2 / 2, H1 + L1 / 2);

      painter.drawLine(p1, p3);
      painter.drawLine(p2, p3);
    } else {
      int L1 = poprect.width() * 1 / 3;
      QRect imageRect = poprect.adjusted(L1, 0, -L1, 0);

      QPixmap draw_pixmap = m_pop_area_arrow_pix.scaled(
          imageRect.width(), imageRect.height(), Qt::KeepAspectRatio);
      painter.drawPixmap(imageRect.center() - draw_pixmap.rect().center(),
                         draw_pixmap);
    }

    //        int sep_line_pos = this->rect().width() * 7 / 8;
    //        // 画竖线
    //        painter.setPen(QPen(Qt::gray, 1)); // 设置画笔颜色和宽度
    //        painter.drawLine(sep_line_pos, rect.top() + 5, sep_line_pos,
    //        rect.bottom() - 5);

    // 在右边区域画箭头
    //        if (m_pop_area_arrow_pix.isNull()) {
    //            painter.setPen(QPen(Qt::gray, 3)); // 设置画笔颜色和宽度
    //            int W1 = sep_line_pos;
    //            int W2 = this->rect().width() - W1;
    //            int L1 = W2 * 1 / 3;
    //            int H1 = this->rect().top() + this->rect().height() / 2;
    //            QPoint p1(W1 + L1, H1);
    //            QPoint p2(W1 + L1 * 2, H1);
    //            QPoint p3(W1 + W2 / 2, H1 + L1 / 2);

    //            painter.drawLine(p1, p3);
    //            painter.drawLine(p2, p3);
    //        }
    //        else {
    //            int W1 = sep_line_pos;
    //            int W2 = this->rect().width() - W1;
    //            int L1 = W2 * 1 / 3;
    //            QRect imageRect = this->rect().adjusted(sep_line_pos + L1, 0,
    //            -L1, 0);

    //            QPixmap draw_pixmap =
    //            m_pop_area_arrow_pix.scaled(imageRect.width(),
    //            imageRect.height(), Qt::KeepAspectRatio);
    //            painter.drawPixmap(imageRect.center() -
    //            draw_pixmap.rect().center(), draw_pixmap);
    //        }
  }
}

QRect MenuToolButton::getPopAreaRect() {
  QRect poprect;
  if (m_pop_area_style == PopAreaStyle::kToolButtonCenterRight) {
    int sep_line_pos = this->rect().width() * 7 / 8;
    poprect = this->rect().adjusted(sep_line_pos, 0, 0, 0);
  } else if (m_pop_area_style == PopAreaStyle::kToolButtonCenterLeft) {
    int sep_line_pos = this->rect().width() * 7 / 8;
    poprect = this->rect().adjusted(0, 0, -sep_line_pos, 0);
  } else if (m_pop_area_style == PopAreaStyle::kToolButtonCornerRight) {
    int sep_line_pos = this->rect().width() * 7 / 8;

    int W1 = sep_line_pos;
    int W2 = this->rect().width() - W1;
    int L1 = W2 * 1 / 3;
    int H1 = this->rect().height() - L1 * 2;

    poprect = this->rect().adjusted(W1, H1, 0, 0);
  } else if (m_pop_area_style == PopAreaStyle::kToolButtonCornerLeft) {
    int sep_line_pos = this->rect().width() * 7 / 8;

    int W1 = sep_line_pos;
    int W2 = this->rect().width() - W1;
    int L1 = W2 * 1 / 3;
    int H1 = this->rect().height() - L1 * 2;

    poprect = this->rect().adjusted(0, H1, -W1, 0);
  }

  return poprect;
}
