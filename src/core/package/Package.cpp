//
// Created by liu on 2016/12/4.
//

#include <QDateTime>
#include "Package.h"
#include "Project.h"
#include "Resourse.h"

namespace TCUIEdit { namespace core { namespace package
{

    const QMultiMap<QString, ui::Function *> Package::m_emptyCategoryMap = QMultiMap<QString, ui::Function *>();

    // Constructors
    //

    Package::Package(Project *project)
    {
        this->init();
        m_proj = project;
    }


    Package::Package(Project *project, const QString &basePath, const QString &name)
    {
        this->init();
        m_proj = project;
        m_basePath = basePath;
        m_name = name;
    }

    Package::~Package()
    {
        delete m_weString;

    }

    // Protected Fcuntions
    //

    void Package::init()
    {
        m_weString = new WEString(this);

        m_base[ui::Base::TRIGGER_CATEGORY] = new Category(this);
        m_base[ui::Base::TRIGGER_TYPE] = new Type(this);
        m_base[ui::Base::TRIGGER_TYPE_DEFAULT] = new TypeDefault(this);
        m_base[ui::Base::TRIGGER_PARAM] = new Param(this);
        m_base[ui::Base::TRIGGER_EVENT] = new Event(this);
        m_base[ui::Base::TRIGGER_CONDITION] = new Condition(this);
        m_base[ui::Base::TRIGGER_ACTION] = new Action(this);
        m_base[ui::Base::TRIGGER_CALL] = new Call(this);

    }

    void Package::_processTrigData(QString &line)
    {
        // Match the sign [\w] when a new ui is assigned.
        QRegExp rx("^\\[\\w+\\]$");
        if (rx.indexIn(line) > -1)
        {
            QString tempStr = rx.cap(0);
            tempStr = tempStr.mid(1, tempStr.length() - 2);
            for (int i = 0; i < ui::Base::TYPE_NUM; i++)
            {
                if (tempStr == ui::Base::typeName(ui::Base::TYPE(i)))
                {
                    if (m_baseTypeCurrent != i)
                    {
                        m_baseTypeCurrent = ui::Base::TYPE(i);
                        m_baseChangedFlag = true;
                    }
                    return;
                }
            }
            /**
             * @TODO throw an error here
             * The name of base type is wrong.
             * We won't change the current base.
             * But if we never have a correct one,
             * there will be a fatal error.
             */
            // m_baseTypeCurrent = ui::Base::UNKNOWN;
            return;
        }
        m_baseChangedFlag = false;


        // Match the comments
        rx.setPattern("^//");
        if (rx.indexIn(line) > -1)
        {
            this->baseCurrent()->readComment(line);
            return;
        }

        if (baseTypeCurrent() < ui::Base::TYPE(8))
        {
            this->baseCurrent()->readLine(line);
        }
    }

    void Package::_processWEStrings(QString &line)
    {
        // Match the sign [\w] when a new ui is assigned.
        QRegExp rx("^\\[\\w+\\]$");
        if (rx.indexIn(line) > -1)return;
        rx.setPattern("^//");
        if (rx.indexIn(line) > -1)
        {
            return;
        }
        m_weString->readLine(line);
    }

    void Package::_writeTrigData()
    {
        for (int i = 0; i <= ui::Base::TRIGGER_CALL; i++)
        {
            m_file.writeLine(m_base[i]->typeDefineText());
            m_file.writeLine();
            m_file.writeLine();
            m_base[i]->writeTrigData(m_file);
            m_file.writeLine();
            m_file.writeLine();
        }
    }

    void Package::_writeWEStrings()
    {
        m_file.writeLine("[WorldEditStrings]");
        m_file.writeLine();
        m_weString->writeWEStrings(m_file);
    }

    // Public Functions
    //

    Project *Package::project() const
    {
        return m_proj;
    }

    WEString *Package::weString() const
    {
        return m_weString;
    }

    Base *Package::base(ui::Base::TYPE type) const
    {
        if (type == ui::Base::UNKNOWN)return NULL;
        return m_base[type];
    }

    Base *Package::baseCurrent() const
    {
        return this->base(m_baseTypeCurrent);
    }

    const ui::Base::TYPE Package::baseTypeCurrent() const
    {
        return m_baseTypeCurrent;
    }

    const bool Package::isBaseChanged() const
    {
        return m_baseChangedFlag;
    }

    const QString &Package::name() const
    {
        return m_name;
    }

    void Package::setName(const QString &name)
    {
        m_name = name;
    }

    void Package::setBasePath(const QString &basePath)
    {
        m_basePath = basePath;
    }

    bool Package::readFile(File::TYPE fileType)
    {
        m_file.open(m_basePath, fileType, true);
        if (m_file.is_open())
        {
            switch (fileType)
            {
            case File::CLASSIC_TRIG_DATA:
                m_processLine = &Package::_processTrigData;
                break;
            case File::CLASSIC_WE_STRINGS:
                m_processLine = &Package::_processWEStrings;
                break;
            default:
                return false;
            }
            return true;
        }
        return false;
    }

    int Package::readLine()
    {
        QString line;
        auto lineNum = m_file.readLineInto(line);
        if (lineNum > 0)
        {
            (this->*m_processLine)(line);
        }
        return lineNum;
    }

    bool Package::writeFile(File::TYPE fileType)
    {
        m_file.open(m_basePath, fileType, false);
        if (m_file.is_open())
        {
            m_file.writeLine(Resourse::license());
            m_file.write("// Generating time: ");
            m_file.writeLine(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            m_file.writeLine();
            m_file.writeLine();
            switch (fileType)
            {
            case File::CLASSIC_TRIG_DATA:
                this->_writeTrigData();
                break;
            case File::CLASSIC_WE_STRINGS:
                this->_writeWEStrings();
                break;
            default:
                return false;
            }
            m_file.close();
            return true;
        }
        return false;
    }

    void Package::addCategoryUI(ui::Function *ui)
    {
        if (ui == NULL)throw ExceptionUndefined();
        if (!ui->isFunction())throw ExceptionTypeError();
        auto name = ui->name();
        int pos = 0;
        while (pos < name.length())
        {
            if (name[pos++] != ' ')break;
        }
        name = name.mid(pos - 1) + name.left(pos - 1);
        name = name.toLower();

        auto category = ui->category();
        category = category.toLower();
        auto &numMap = m_proj->categoryNumMap();
        auto it = numMap.find(category);
        if (it != numMap.end())(*it).second++;
        else numMap.insert(category, qMakePair(false, 1));

        QString returnType = "";
        if (ui->type() == ui::Base::TRIGGER_CALL)
        {
            returnType = ((ui::Call *) ui)->returnType().toLower();
            auto &_numMap = m_proj->typeNumMap();
            it = _numMap.find(returnType);
            if (it != _numMap.end())(*it).second++;
            else _numMap.insert(returnType, qMakePair(false, 1));
        }
        auto &map = m_categoryMap[ui->functionType()][qMakePair(category, returnType)];
        if (map.find(name, ui) == map.end())
        {
            map.insert(name, ui);
        }
    }

    void Package::removeCategoryUI(ui::Function *ui)
    {
        if (ui == NULL)throw ExceptionUndefined();
        if (!ui->isFunction())throw ExceptionTypeError();
        auto funcType = ui->functionType();

        auto category = ui->category();
        category = category.toLower();
        auto &numMap = m_proj->categoryNumMap();
        auto it = numMap.find(category);
        if (it != numMap.end())(*it).second--;

        QString returnType = "";
        if (ui->type() == ui::Base::TRIGGER_CALL)
        {
            returnType = ((ui::Call *) ui)->returnType().toLower();
            auto &_numMap = m_proj->typeNumMap();
            it = _numMap.find(returnType);
            if (it != _numMap.end())(*it).second--;
        }

        auto _it = m_categoryMap[funcType].find(qMakePair(category, returnType));
        if (_it != m_categoryMap[funcType].end())
        {
            auto name = ui->name();
            int pos = 0;
            while (pos < name.length())
            {
                if (name[pos++] != ' ')break;
            }
            name = name.mid(pos - 1) + name.left(pos - 1);
            name = name.toLower();
            (*_it).remove(name, ui);
        }
    }

    const QHash<QPair<QString, QString>, QMultiMap<QString, ui::Function * >> &
    Package::categoryMap(ui::Function::FUNCTION_TYPE funcType) const
    {
        return m_categoryMap[funcType];
    }

    const QMultiMap<QString, ui::Function *> &
    Package::categoryMap(ui::Function::FUNCTION_TYPE funcType, const QString &category, const QString &returnType) const
    {
        auto it = m_categoryMap[funcType].find(qMakePair(category, returnType));
        if (it == m_categoryMap[funcType].end())
            return m_emptyCategoryMap;
        return (*it);
    }
}}}

