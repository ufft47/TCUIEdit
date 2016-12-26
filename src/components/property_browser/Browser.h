//
// Created by liu on 2016/12/21.
//

#pragma once

#include "common.h"
#include "Model.h"
#include "Category.h"

namespace TCUIEdit { namespace property_browser
{

    class Browser : public QTreeView
    {
    Q_OBJECT
    protected:
        Model *m_model;

        QMap<QString, Row *> m_propertyMap;

        void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                         const QVector<int> &roles = QVector<int>());


    public:
        Browser(QWidget *parent = Q_NULLPTR);

        ~Browser();

        void init();

        bool addAlias(Row *item, const QString &alias);

        Row *aliasRow(const QString &alias);

        Category *addCategory(const QString &text, const QString &alias = "");


    };

}}


