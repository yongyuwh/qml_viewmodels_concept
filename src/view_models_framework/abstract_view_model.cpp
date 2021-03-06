#include "abstract_view_model.hpp"

#include <QList>


namespace ViewModelsFramework
{


AbstractViewModel::AbstractViewModel(QObject* parent) : QObject(parent),
    m_modified(false),
    m_isUpdatingView(false)
{
}


bool AbstractViewModel::modified() const
{
    return m_modified;
}


void AbstractViewModel::setModified(bool value)
{
    if (m_modified != value)
    {
        m_modified = value;
        emit modifiedChanged();
    }
}


bool AbstractViewModel::isUpdatingView() const
{
    return m_isUpdatingView;
}


void AbstractViewModel::updateView()
{
    // Avoid reentrant calls
    if (m_isUpdatingView)
    {
        return;
    }

    m_isUpdatingView = true;

    // Get a list of child fields
    QList<FieldBackendBase*> fields = QObject::findChildren<FieldBackendBase*>(QString(), Qt::FindDirectChildrenOnly);

    // Prepare fields for the update
    for (FieldBackendBase* field : fields)
    {
        field->blockSignals(true);
        field->beginFieldUpdate();
    }

    try
    {
        // Let the derived class update the view
        do_updateView();
    }
    catch (...)
    {
        qDebug("AbstractViewModel::updateView() - Exception caught calling do_updateView() !!!");
    }

    // Finalize fields update
    for (FieldBackendBase* field : fields)
    {
        field->blockSignals(false);
        field->endFieldUpdate();
    }

    m_isUpdatingView = false;
}


void AbstractViewModel::onFieldChanged()
{
    setModified(true);
    updateView();
}


void AbstractViewModel::do_updateView()
{
}


bool AbstractViewModel::areFieldsChangedFromOriginalValues() const
{
    QList<FieldBackendBase*> fields = QObject::findChildren<FieldBackendBase*>(QString(), Qt::FindDirectChildrenOnly);
    for (const FieldBackendBase* field : fields)
    {
        if (field->isChangedFromOriginalValue())
        {
            return true;
        }
    }

    return false;
}


void AbstractViewModel::restoreOriginalFieldValues()
{
    // Avoid reentrant calls
    if (m_isUpdatingView)
    {
        return;
    }

    m_isUpdatingView = true;

    // Get a list of child fields
    QList<FieldBackendBase*> fields = QObject::findChildren<FieldBackendBase*>(QString(), Qt::FindDirectChildrenOnly);

    // Prepare fields for the update and restore original values
    for (FieldBackendBase* field : fields)
    {
        field->blockSignals(true);
        field->beginFieldUpdate();
        field->restoreOriginalValue();
    }

    try
    {
        // Let the derived class update the view
        do_updateView();
    }
    catch (...)
    {
        qDebug("AbstractViewModel::updateView() - Exception caught calling do_updateView() !!!");
    }

    // Finalize fields update
    for (FieldBackendBase* field : fields)
    {
        field->blockSignals(false);
        field->endFieldUpdate();
    }

    m_isUpdatingView = false;
}


void AbstractViewModel::clearFieldsMessages()
{
    QList<FieldBackendBase*> fields = QObject::findChildren<FieldBackendBase*>(QString(), Qt::FindDirectChildrenOnly);
    for (FieldBackendBase* field : fields)
    {
        field->statusMessages()->clearMessages();
    }
}


void AbstractViewModel::deleteFieldsMessagesBySeverity(FieldMessage::MessageSeverity minSeverityToDelete)
{
    QList<FieldBackendBase*> fields = QObject::findChildren<FieldBackendBase*>(QString(), Qt::FindDirectChildrenOnly);
    for (FieldBackendBase* field : fields)
    {
        field->statusMessages()->deleteMessagesBySeverity(minSeverityToDelete);
    }
}


void AbstractViewModel::addFieldMessageByFieldId(int fieldId, FieldMessage::MessageSeverity severity, const QString& message)
{
    QList<FieldBackendBase*> fields = QObject::findChildren<FieldBackendBase*>(QString(), Qt::FindDirectChildrenOnly);
    for (FieldBackendBase* field : fields)
    {
        if (field->fieldId() == fieldId)
        {
            field->statusMessages()->addMessage(severity, message);
        }
    }
}


} // namespace ViewModelsFramework
