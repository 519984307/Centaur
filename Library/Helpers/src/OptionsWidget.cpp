/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 03/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "OptionsWidget.hpp"

CENTAUR_NAMESPACE::OptionsWidget::OptionsWidget(int buttons, QWidget *parent) :
    QWidget { parent }
{
    initWidget();

    if (!(buttons & Buttons::view))
        m_ui.viewBtn->hide();
    if (!(buttons & Buttons::edit))
        m_ui.editBtn->hide();
    if (!(buttons & Buttons::del))
        m_ui.delBtn->hide();
}

QPushButton *CENTAUR_NAMESPACE::OptionsWidget::editButton() const noexcept
{
    return m_ui.editBtn;
}

QPushButton *CENTAUR_NAMESPACE::OptionsWidget::deleteButton() const noexcept
{
    return m_ui.delBtn;
}

QPushButton *CENTAUR_NAMESPACE::OptionsWidget::viewButton() const noexcept
{
    return m_ui.viewBtn;
}

void cen::OptionsWidget::initWidget()
{
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Options"));
    resize(400, 300);
    m_ui.horizontalLayout = new QHBoxLayout(this);
    m_ui.horizontalLayout->setSpacing(5);
    m_ui.horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    m_ui.horizontalLayout->setContentsMargins(0, 0, 0, 0);
    m_ui.horizontalSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_ui.horizontalLayout->addItem(m_ui.horizontalSpacerLeft);

    m_ui.viewBtn = new QPushButton(this);
    m_ui.viewBtn->setObjectName(QString::fromUtf8("viewBtn"));
    m_ui.viewBtn->setMinimumSize(QSize(20, 20));
    m_ui.viewBtn->setMaximumSize(QSize(20, 20));
    m_ui.viewBtn->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                  "color: black;\n"
                                                  "border: 0px;\n"
                                                  "border-radius: 5px;\n"
                                                  "background-color: rgb(68, 138, 255);\n"
                                                  "border-image: url(:/list-items/view) 0 0 0 0;\n"
                                                  "}\n"
                                                  "\n"
                                                  "QPushButton:hover, QPushButton:hover:default{\n"
                                                  "	background-color:rgb(83,147,255);\n"
                                                  "}\n"
                                                  "QPushButton:pressed{\n"
                                                  "background-color: rgb(93,157,255);\n"
                                                  "}\n"
                                                  "QPushButton:default, QPushButton:pressed:default{\n"
                                                  "background-color: rgb(93,157,255);\n"
                                                  "}\n"
                                                  "QPushButton:disabled{\n"
                                                  "color: #c0c0c0;\n"
                                                  "background-color: #9D9EA4;\n"
                                                  "}\n"
                                                  "\n"
                                                  ""));

    m_ui.horizontalLayout->addWidget(m_ui.viewBtn);

    m_ui.editBtn = new QPushButton(this);
    m_ui.editBtn->setObjectName(QString::fromUtf8("editBtn"));
    m_ui.editBtn->setMinimumSize(QSize(20, 20));
    m_ui.editBtn->setMaximumSize(QSize(20, 20));
    m_ui.editBtn->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                  "color: black;\n"
                                                  "border: 0px;\n"
                                                  "border-radius: 5px;\n"
                                                  "background-color: rgb(68, 138, 255);\n"
                                                  "border-image: url(:/list-items/edit) 0 0 0 0;\n"
                                                  "}\n"
                                                  "\n"
                                                  "QPushButton:hover, QPushButton:hover:default{\n"
                                                  "	background-color:rgb(83,147,255);\n"
                                                  "}\n"
                                                  "QPushButton:pressed{\n"
                                                  "background-color: rgb(93,157,255);\n"
                                                  "}\n"
                                                  "QPushButton:default, QPushButton:pressed:default{\n"
                                                  "background-color: rgb(93,157,255);\n"
                                                  "}\n"
                                                  "QPushButton:disabled{\n"
                                                  "color: #c0c0c0;\n"
                                                  "background-color: #9D9EA4;\n"
                                                  "}\n"
                                                  "\n"
                                                  ""));

    m_ui.horizontalLayout->addWidget(m_ui.editBtn);

    m_ui.delBtn = new QPushButton(this);
    m_ui.delBtn->setObjectName(QString::fromUtf8("delBtn"));
    m_ui.delBtn->setMinimumSize(QSize(20, 20));
    m_ui.delBtn->setMaximumSize(QSize(20, 20));
    m_ui.delBtn->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                 "	border-radius: 5px;\n"
                                                 "	background-color: #B00020;\n"
                                                 "	border-image: url(:/list-items/delete) 0 0 0 0;\n"
                                                 "}\n"
                                                 "\n"
                                                 "QPushButton:hover, QPushButton:hover:default{\n"
                                                 "	background-color: #E03755;\n"
                                                 "}\n"
                                                 "QPushButton:pressed{\n"
                                                 "background-color:  #CF6679;\n"
                                                 "}\n"
                                                 "QPushButton:default, QPushButton:pressed:default{\n"
                                                 "background-color:  #CF6679;\n"
                                                 "}\n"
                                                 ""));

    m_ui.horizontalLayout->addWidget(m_ui.delBtn);

    m_ui.horizontalSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_ui.horizontalLayout->addItem(m_ui.horizontalSpacerRight);

    QMetaObject::connectSlotsByName(this);
}
