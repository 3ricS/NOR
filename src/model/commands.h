/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein Commands ist für das Undo, Redo zuständig.
 *
 * Ein Command ist ein QUndoCommand.
 * Die Methoden werden entsprechend für die einzelnen Aktionen, die beim Bearbeiten des Netzwerkes getätigt werden, überladen.
 * Die Zeiger auf entfernte Elemente bleiben so lange erhalten, bis die QUndoCommands vom QUndoStack entfernt werden.
 */
#ifndef NOR_COMMANDS_H
#define NOR_COMMANDS_H

#include <QUndoCommand>
#include <QPointF>

#include <model/component.h>
#include <model/componentport.h>

class NetworkGraphics;
class Connection;
class Description;

class CommandAddComponent : public QUndoCommand
{
public:
    CommandAddComponent(NetworkGraphics* model, QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);
    ~CommandAddComponent(void);

    void undo(void) override;
    void redo(void) override;

    Component* getCreatedComponent(void) {return _createdComponent;}

private:
    Component*               _createdComponent = nullptr;
    NetworkGraphics*         _model = nullptr;
    QPointF                  _gridPosition;
    Component::ComponentType _componentType;
    bool                     _componentIsVertical;
    QList<Connection*>       _deletedConnections;
    bool                     _hasDoneUndo = false;
};



class CommandAddConnection : public QUndoCommand
{
public:
    CommandAddConnection(NetworkGraphics* model, ComponentPort componentPortA, ComponentPort componentPortB);
    ~CommandAddConnection(void);

    void undo(void) override;
    void redo(void) override;

private:
    NetworkGraphics* _model = nullptr;
    Connection*      _createdConnection = nullptr;
    ComponentPort    _componentPortA;
    ComponentPort    _componentPortB;
    bool             _hasDoneUndo = false;
};



class CommandAddDescriptionField : public QUndoCommand
{
public:
    CommandAddDescriptionField(NetworkGraphics* model, Description* descriptionField);
    ~CommandAddDescriptionField(void);

    void undo(void) override;
    void redo(void) override;

private:
    NetworkGraphics* _model = nullptr;
    Description*     _createdDescriptionField = nullptr;
    bool             _hasDoneUndo = false;
};




class CommandMoveComponent : public QUndoCommand
{
public:
    CommandMoveComponent(NetworkGraphics* model, GridObject* objectToMove, QPointF newGridPosition);

    void undo(void) override;
    void redo(void) override;

private:
    NetworkGraphics* _model = nullptr;
    GridObject*      _objectToMove = nullptr;
    QPointF          _gridEndPosition;
    QPointF          _gridStartPosition;
};



class CommandEditComponent : public QUndoCommand {
public:
    CommandEditComponent(NetworkGraphics* model, Component* editedComponent,
                         Component::Orientation originalOrientation, QString newName,
                         double newValue);

    void undo(void) override;
    void redo(void) override;

private:
    NetworkGraphics*       _model = nullptr;
    Component*             _editedComponent = nullptr;
    QString                _newName;
    QString                _oldName;
    long double            _newValue;
    long double            _oldValue;
    Component::Orientation _oldOrientation;
    Component::Orientation _newOrientation;
};



class CommandDeleteComponent : public QUndoCommand
{
public:
    CommandDeleteComponent(NetworkGraphics* model, Component* componentToDelete);
    ~CommandDeleteComponent(void);

    void undo(void) override;
    void redo(void) override;

private:
    NetworkGraphics*   _model = nullptr;
    Component*         _deletedComponent = nullptr;
    QList<Connection*> _deletedConnections;
    bool               _hasDoneUndo = false;
};



class CommandDeleteConnection : public QUndoCommand
{
public:
    CommandDeleteConnection(NetworkGraphics* model, Connection* connectionToDelete);
    ~CommandDeleteConnection(void);

    void undo(void) override;
    void redo(void) override;

private:
    NetworkGraphics* _model = nullptr;
    Connection*      _deletedConnection = nullptr;
    bool             _hasDoneUndo = false;
};



class CommandDeleteDescription : public QUndoCommand
{
public:
    CommandDeleteDescription(NetworkGraphics* model, Description* descriptionField);
    ~CommandDeleteDescription(void);

    void undo(void) override;
    void redo(void) override;

private:
    NetworkGraphics* _model = nullptr;
    Description*     _deletedDescription = nullptr;
    bool             _hasDoneUndo = false;
};



class CommandDuplicateComponent : public QUndoCommand
{
public:
    CommandDuplicateComponent(NetworkGraphics* model, Component* componentToDuplicate, int xPosition, int yPosition);
    ~CommandDuplicateComponent(void) {};

    void undo(void) override;
    void redo(void) override;

    Component* getCreatedComponent(void) {return _createdComponent;}

private:
    Component*       _createdComponent = nullptr;
    NetworkGraphics* _model = nullptr;
    Component*       _componentToDuplicate = nullptr;
    int              _xPosition;
    int              _yPosition;
};



class CommandDuplicateDescription : public QUndoCommand
{
public:
    CommandDuplicateDescription(NetworkGraphics* model, Description* descriptionToDuplicate, int xPosition, int yPosition);
    ~CommandDuplicateDescription(void) {};

    void undo(void) override;
    void redo(void) override;

    Description* getCreatedDescription() {return _createdDescription;}

private:
    Description*     _createdDescription = nullptr;
    NetworkGraphics* _model = nullptr;
    Description*     _descriptionToDuplicate = nullptr;
    int              _xPosition;
    int              _yPosition;
};



class CommandEditDescription : public QUndoCommand
{
public:
    CommandEditDescription(NetworkGraphics* model, Description* descriptionFieldToEdit, QString newText);
    ~CommandEditDescription(void) {};

    void undo(void) override;
    void redo(void) override;

private:
    Description*     _editDescription = nullptr;
    NetworkGraphics* _model = nullptr;
    QString          _newText;
    QString          _oldText;
};



class CommandRotateComponent : public QUndoCommand
{
public:
    CommandRotateComponent(Component* ComponentToTurn, NetworkGraphics* model);
    ~CommandRotateComponent(void) {};

    void undo(void) override;
    void redo(void) override;


private:
    Component*       _componentToTurn = nullptr;
    NetworkGraphics* _model = nullptr;
};




#endif //NOR_COMMANDS_H
