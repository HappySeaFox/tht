#include "qdropboxjson.h"

QDropboxJson::QDropboxJson(QObject *parent) :
    QObject(parent)
{
    _init();
}

QDropboxJson::QDropboxJson(QString strJson, QObject *parent) :
    QObject(parent)
{
    _init();
    parseString(strJson);
}

QDropboxJson::QDropboxJson(const QDropboxJson &other) :
    QObject(other.parent())
{
    _init();
    parseString(other.strContent());
}

QDropboxJson::~QDropboxJson()
{
    emptyList();
}

void QDropboxJson::_init()
{
	valid          = false;
	_anonymousArray = false;
}

void QDropboxJson::parseString(QString strJson)
{
#ifdef QTDROPBOX_DEBUG
    qDebug() << "parse string = " << strJson << endl;
#endif

    // clear all existing data
    emptyList();

    // basically a json is valid until it is invalidated
    valid = true;

    if(!strJson.startsWith("{") ||
            !strJson.endsWith("}"))
    {
#ifdef QTDROPBOX_DEBUG
    qDebug() << "string does not start with { " << endl;
#endif

		if(strJson.startsWith("[") && strJson.endsWith("]"))
		{
#ifdef QTDROPBOX_DEBUG
			qDebug() << "JSON is anonymous array" << endl;
#endif
			_anonymousArray = true;
			// fix json to be parseable by the algorithm below
			strJson = "{\"_anonArray\":"+strJson+"}";
		}
		else
		{
			valid = false;
			return;
		}
    }

    QString buffer   = "";
    QString key      = "";
    QString value    = "";

    bool isKey       = true;
    bool insertValue = false;
    bool isJson      = false;
    bool isArray     = false;
    bool openQuotes  = false;


    for(int i=0; i<strJson.size(); ++i)
    {
        switch(strJson.at(i).toLatin1())
        {
        case '"':
            if(!isKey)
            {
                buffer += "\"";
                openQuotes = !openQuotes;
            }
            continue;
            break;
        case ' ':
            if(!isKey)
                buffer += " ";
            continue;
            break;
        case '}':
            continue;
            break;
        case ':':
            if(!isKey)
            {
                buffer += ":";
                continue;
            }
#ifdef QTDROPBOX_DEBUG
            qDebug() << "key = " << buffer << endl;
#endif

            key    = buffer.trimmed();
            buffer = "";
            isKey  = false;
            break;
        case ',':
            if(openQuotes)
			{
				buffer += ',';
                continue;
			}
#ifdef QTDROPBOX_DEBUG
            qDebug() << "value = " << buffer << endl;
#endif
            value       = buffer.trimmed();
            buffer      = "";
            isKey       = true;
            insertValue = true;
            break;
        case '{':
            if(i == 0)
                continue;
            isJson  = true;
            buffer += '{';
            break;
        case '[':
            isArray = true;
            buffer += '[';
            break;
        default:
            buffer += strJson.at(i);
            break;
        }

        if(isJson)
        {
            // create new json object with data until }
            qdropboxjson_entry e;
			int offset = parseSubJson(strJson, i, &e);
            valueMap[key] = e;
            key = "";

            // ignore next ,
            i = offset+1;
            buffer      = "";
            isJson      = false;
            insertValue = false;
            isKey       = true;
            //continue;
        }

		 if(isArray)
         {
			 // value is an array value -> parse array
			 bool inString    = false;
			 bool arrayEnd    = false;
			 int j = i+1;
			 buffer = "[";
			 for(;!arrayEnd && j<strJson.size();++j)
			 {
				 QChar arrC = strJson.at(j);
				 switch(arrC.toLatin1())
				 {
				 case '"': 
					 inString = !inString;
					 buffer += arrC;
					 break;
				 case ']':
					 buffer += "]";
					 if(!inString)
						 arrayEnd = true;
					 break;
				 case '\\':
					 if(strJson.at(j+1) == '"') // escaped double quote
					 {
						 buffer += "\"";
						 j++;
					 }
					 else
						 buffer += "\\";
					 break;
				 default:
					 buffer += arrC;
					 break;
				 }
			 }
			 
			 // save array value string (buffer)
			 value       = buffer;
			 buffer      = "";
			 i           = j;
			 insertValue = true;
			 isArray     = false;
			 isKey       = true; // next element is started
		 }

        if(insertValue)
        {
#ifdef QTDROPBOX_DEBUG
            qDebug() << "insert value " << key << " with content = " << value.trimmed() << " and type = " << interpretType(value.trimmed()) << endl;
#endif
            qdropboxjson_entry e;
			QString *valuePointer = new QString(value.trimmed());
			e.value.value = valuePointer;
            e.type        = interpretType(value.trimmed());
            valueMap[key] = e;

            key   = "";
            value = "";
            insertValue = false;
        }
    }

    // there's some key left
    if(key.compare(""))
    {
#ifdef QTDROPBOX_DEBUG
            qDebug() << "rest value = " << buffer << endl;
#endif
        // but no value in the buffer -> json is invalid because a value is missing
        if(!buffer.compare(""))
        {
            valid = false;
        }
        else
        {
            // if there's a value left we store it
            qdropboxjson_entry e;
            e.value.value = new QString(buffer.trimmed());
            e.type        = interpretType(buffer.trimmed());
            valueMap[key] = e;
        }

    }

    return;
}

void QDropboxJson::clear()
{
    emptyList();
    return;
}

bool QDropboxJson::isValid()
{
    return valid;
}

bool QDropboxJson::hasKey(QString key)
{
    return valueMap.contains(key);
}

QDropboxJson::DataType QDropboxJson::type(QString key)
{
    if(!valueMap.contains(key))
        return UnknownType;

    qdropboxjson_entry_type t;
    t = valueMap.value(key).type;

    switch(t)
    {
    case QDROPBOXJSON_TYPE_NUM:
        return NumberType;
    case QDROPBOXJSON_TYPE_STR:
        return StringType;
    case QDROPBOXJSON_TYPE_JSON:
        return JsonType;
    case QDROPBOXJSON_TYPE_ARRAY:
        return ArrayType;
    case QDROPBOXJSON_TYPE_FLOAT:
        return FloatType;
    case QDROPBOXJSON_TYPE_BOOL:
        return BoolType;
    case QDROPBOXJSON_TYPE_UINT:
        return UnsignedIntType;
    default:
        return UnknownType;
    }
    return UnknownType;
}

qint64 QDropboxJson::getInt(QString key, bool force)
{
    if(!valueMap.contains(key))
        return 0;

    qdropboxjson_entry e;
    e = valueMap.value(key);

    if(!force && e.type != QDROPBOXJSON_TYPE_NUM)
        return 0;

    return e.value.value->toInt();
}

void QDropboxJson::setInt(QString key, qint64 value)
{
    if(valueMap.contains(key)){
        valueMap[key].value.value->setNum(value);
    }else{
        qdropboxjson_entry e;
        QString *valuePointer = new QString();
        valuePointer->setNum(value);
        e.value.value = valuePointer;
        e.type        = QDROPBOXJSON_TYPE_NUM;
        valueMap[key] = e;
    }
}

quint64 QDropboxJson::getUInt(QString key, bool force)
{
    if(!valueMap.contains(key))
        return 0;

    qdropboxjson_entry e;
    e = valueMap.value(key);

    if(!force && e.type != QDROPBOXJSON_TYPE_UINT)
        return 0;

    return e.value.value->toUInt();
}

void QDropboxJson::setUInt(QString key, quint64 value)
{
    if(valueMap.contains(key)){
        valueMap[key].value.value->setNum(value);
    }else{
        qdropboxjson_entry e;
        QString *valuePointer = new QString();
        valuePointer->setNum(value);
        e.value.value = valuePointer;
        e.type        = QDROPBOXJSON_TYPE_UINT;
        valueMap[key] = e;
    }
}

QString QDropboxJson::getString(QString key, bool force)
{
    if(!valueMap.contains(key))
        return "";

    qdropboxjson_entry e;
    e = valueMap.value(key);

    if(!force && e.type != QDROPBOXJSON_TYPE_STR)
        return "";

	QString value = e.value.value->mid(1, e.value.value->size()-2);
    return value;
}

void QDropboxJson::setString(QString key, QString value)
{
    if(valueMap.contains(key)){
        *(valueMap[key].value.value) = value;
    }else{
        qdropboxjson_entry e;
        QString *valuePointer = new QString(value);
        e.value.value = valuePointer;
        e.type        = QDROPBOXJSON_TYPE_STR;
        valueMap[key] = e;
    }
}

QDropboxJson* QDropboxJson::getJson(QString key)
{
    if(!valueMap.contains(key))
        return NULL;

    qdropboxjson_entry e;
    e = valueMap.value(key);

    if(e.type != QDROPBOXJSON_TYPE_JSON)
        return NULL;


    return e.value.json;
}

void QDropboxJson::setJson(QString key, QDropboxJson value)
{
    if(valueMap.contains(key)){
        *(valueMap[key].value.json) = value;
    }else{
        qdropboxjson_entry e;
        QDropboxJson *valuePointer = new QDropboxJson(value);
        e.value.json = valuePointer;
        e.type        = QDROPBOXJSON_TYPE_JSON;
        valueMap[key] = e;
    }
}

double QDropboxJson::getDouble(QString key, bool force)
{
    if(!valueMap.contains(key))
        return 0.0f;

    qdropboxjson_entry e;
    e = valueMap.value(key);

    if(!force && e.type != QDROPBOXJSON_TYPE_FLOAT)
        return 0.0f;

    return e.value.value->toDouble();
}

void QDropboxJson::setDouble(QString key, double value)
{
    if(valueMap.contains(key)){
        valueMap[key].value.value->setNum(value);
    }else{
        qdropboxjson_entry e;
        QString *valuePointer = new QString();
        valuePointer->setNum(value);
        e.value.value = valuePointer;
        e.type        = QDROPBOXJSON_TYPE_FLOAT;
        valueMap[key] = e;
    }
}

bool QDropboxJson::getBool(QString key, bool force)
{
    if(!valueMap.contains(key))
        return false;

    qdropboxjson_entry e;
    e = valueMap.value(key);

    if(!force && e.type != QDROPBOXJSON_TYPE_BOOL)
        return false;

    if(!e.value.value->compare("false"))
        return false;

    return true;
}

void QDropboxJson::setBool(QString key, bool value)
{
    if(valueMap.contains(key)){
        *(valueMap[key].value.value) = value ? "true" : "false";
    }else{
        qdropboxjson_entry e;
        QString *valuePointer = new QString(value ? "true" : "false");
        e.value.value = valuePointer;
        e.type        = QDROPBOXJSON_TYPE_BOOL;
        valueMap[key] = e;
    }
}

QDateTime QDropboxJson::getTimestamp(QString key, bool force)
{
	if(!valueMap.contains(key))
		return QDateTime();

	qdropboxjson_entry e;
	e = valueMap.value(key);

	if(!force && e.type != QDROPBOXJSON_TYPE_STR)
		return QDateTime();

    // Dropbox date time format "Sat, 21 Aug 2010 22:31:20 +0000"
    QString day   = e.value.value->mid(1, 3);
    QString part1 = e.value.value->mid(6, 2);
    QString month = e.value.value->mid(9, 3);
    QString part2 = e.value.value->mid(13,13);
	        month = translateMonth(month);
            day   = translateDay(day);
    QString dval = QString("%1 %2 %3 %4").arg(day).arg(part1).arg(month).arg(part2);

    return QDateTime::fromString(dval, "ddd dd MMM yyyy hh:mm:ss");
}

void QDropboxJson::setTimestamp(QString key, QDateTime value)
{
    QString valueToSave;
    valueToSave += translateDay(value.date().dayOfWeek()) + " ";
    valueToSave += QString::number(value.date().day()) + " ";
    valueToSave += translateMonth(value.date().month()) + " ";
    valueToSave += QString::number(value.date().year()) + " ";
    valueToSave += value.toString("hh:mm:ss");

    if(valueMap.contains(key)){
        *(valueMap[key].value.value) = valueToSave;
    }else{
        qdropboxjson_entry e;
        QString *valuePointer = new QString(valueToSave);
        e.value.value = valuePointer;
        e.type        = QDROPBOXJSON_TYPE_STR;
        valueMap[key] = e;
    }
}

QString QDropboxJson::strContent() const
{
	if(valueMap.size() == 0)
		return "";

    QString content = "{";
	QList<QString> keys = valueMap.keys();
	for(int i=0; i<keys.size(); ++i)
	{
		QString value = "";
		qdropboxjson_entry e = valueMap.value(keys.at(i));

		if(e.type != QDROPBOXJSON_TYPE_JSON)
			value = *e.value.value;
		else
			value = e.value.json->strContent();

		content.append(QString("\"%1\": %2").arg(keys.at(i)).arg(value));
		if(i != keys.size()-1)
			content.append(", ");
	}
	content.append("}");
	return content;
}

void QDropboxJson::emptyList()
{
    QList<QString> keys = valueMap.keys();
    for(qint32 i=0; i<keys.size(); ++i)
    {
        qdropboxjson_entry e = valueMap.value(keys.at(i));
        if(e.type == QDROPBOXJSON_TYPE_JSON)
            delete e.value.json;
        else
            delete e.value.value;
		valueMap.remove(keys.at(i));
    }
    valueMap.empty();
    return;
}

qdropboxjson_entry_type QDropboxJson::interpretType(QString value)
{
    // check for string
    if(value.startsWith("\"") && value.endsWith("\""))
        return QDROPBOXJSON_TYPE_STR;

    // check for integer
    bool ok;
    value.toInt(&ok);
    if(ok)
        return QDROPBOXJSON_TYPE_NUM;

    // check for uint
    value.toUInt(&ok);
    if(ok)
        return QDROPBOXJSON_TYPE_UINT;

    // check for bool
    if(!value.compare("true") || !value.compare("false"))
        return QDROPBOXJSON_TYPE_BOOL;

    // check for array
    if(value.startsWith("[") && value.endsWith("]"))
        return QDROPBOXJSON_TYPE_ARRAY;

    value.toDouble(&ok);
    if(ok)
        return QDROPBOXJSON_TYPE_FLOAT;

    return QDROPBOXJSON_TYPE_UNKNOWN;
}

QString QDropboxJson::translateMonth(QString month)
{
	QStringList months;
	months << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun"
		   << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";

	for(int i=1; i<=12; ++i)
	{
		if(months.at(i-1).compare(month) == 0)
			return QDate::shortMonthName(i);
	}

    return "<unknown>";
}

QString QDropboxJson::translateMonth(int month)
{
    QStringList months;
    months << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun"
           << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";

    if(month > 0 && (month-1) < months.size()){
        return months.at(month-1);
    }

    return "<unknown>";
}

QString QDropboxJson::translateDay(QString day)
{
	QStringList days;
	days << "Mon" << "Tue" << "Wed" << "Thu" << "Fri" << "Sat" << "Sun";

	for(int i=1; i<=7; ++i)
	{
		if(days.at(i-1).compare(day) == 0)
			return QDate::shortDayName(i);
	}

    return "<unknown>";
}

QString QDropboxJson::translateDay(int day)
{
    QStringList days;
    days << "Mon" << "Tue" << "Wed" << "Thu" << "Fri" << "Sat" << "Sun";

    if(day > 0 && (day-1) < days.size()){
        return days.at(day-1);
    }

    return "<unknown>";
}

QDropboxJson& QDropboxJson::operator=(QDropboxJson& other)
{
	/*!< \todo use toString() */
	parseString(other.strContent());
	return *this;
}

QStringList QDropboxJson::getArray(QString key, bool force)
{
	QStringList list;
	if(!valueMap.contains(key))
        return list;

    qdropboxjson_entry e;
    e = valueMap.value(key);

    if(!force && e.type != QDROPBOXJSON_TYPE_ARRAY)
        return list;

	QString arrayStr = e.value.value->mid(1, e.value.value->length()-2);
	QString buffer = "";
	bool inString = false;
	int  inJson   = 0;
	for(int i=0; i<arrayStr.size(); ++i)
	{
		QChar c = arrayStr.at(i);
		if( ((c != ',' && c != ' ') || inString || inJson) &&
			(c != '"' || inJson > 0))
			buffer += c;
		switch(c.toLatin1())
		{
		case '"':
			if(i > 0 && arrayStr.at(i-1).toLatin1() == '\\')
			{
				buffer += c;
				break;
			}
			else
				inString = !inString;
			break;
		case '{':
			inJson++;
			break;
		case '}':
			inJson--;
			break;
		case ',':
			if(inJson == 0 && !inString)
			{
				list.append(buffer);
				buffer = "";
			}
			break;
		}
	}

	if(!buffer.isEmpty())
		list.append(buffer);

    return list;
}

int QDropboxJson::parseSubJson(QString strJson, int start, qdropboxjson_entry *jsonEntry)
{
	int openBrackets = 1;
	QString buffer = "";
    QDropboxJson* jsonValue = NULL;

	int j;
	for(j=start+1; openBrackets > 0 && j < strJson.size(); ++j)
	{
		if(strJson.at(j).toLatin1() == '{')
			openBrackets++;
		else if(strJson.at(j).toLatin1() == '}')
			openBrackets--;
	}

	buffer = strJson.mid(start, j-start);
#ifdef QTDROPBOX_DEBUG
	qDebug() << "brackets = " << openBrackets << endl;
	qDebug() << "json data(" << start << ":" << j-start << ") = " << buffer << endl;
#endif
	jsonValue = new QDropboxJson();
	jsonValue->parseString(buffer);

	// invalid sub json means invalid json
	if(!jsonValue->isValid())
	{
#ifdef QTDROPBOX_DEBUG
		qDebug() << "subjson invalid!" << endl;
#endif
		valid = false;
		return j;
	}

	// insert new
	jsonEntry->value.json = jsonValue;
	jsonEntry->type       = QDROPBOXJSON_TYPE_JSON;
	return j;
}

bool QDropboxJson::isAnonymousArray()
{
	return _anonymousArray;
}

QStringList QDropboxJson::getArray()
{
	if(!isAnonymousArray())
		return QStringList();

	return getArray("_anonArray");
}

int QDropboxJson::compare(const QDropboxJson& other)
{
	if(valueMap.size() != other.valueMap.size())
		return 1;

	QMap<QString, qdropboxjson_entry> yourMap = other.valueMap;

	QList<QString> keys = valueMap.keys();
	for(int i=0; i<keys.size(); ++i)
	{
		QString key = keys.at(i);
		if(!yourMap.contains(key))
			return 1;

		qdropboxjson_entry myEntry = valueMap.value(key);
		qdropboxjson_entry yourEntry = yourMap.value(key);
		
		if(myEntry.type != yourEntry.type)
			return 1;

		if(myEntry.type == QDROPBOXJSON_TYPE_JSON)
		{
			if(myEntry.value.json->compare(*yourEntry.value.json) != 0)
				return 1;
		}
		else
		{
			if(myEntry.value.value->compare(yourEntry.value.value) != 0)
				return 1;
		}
	}

	return 0;
}
