import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window

ApplicationWindow {
    id: win
    // A share of the screen rather than fixed pixels, so the note keeps its
    // apparent size at any monitor scale; capped so it stays a sticky note
    // on large displays. Screen dimensions are logical, matching the cap.
    width: Math.min(640, Math.round(Screen.width * 0.36))
    height: Math.min(520, Math.round(Screen.height * 0.45))
    minimumWidth: 240
    minimumHeight: 180
    visible: true
    title: "Barq"

    readonly property bool darkMode: backend.darkMode
    readonly property color pageColor: backend.themeBackground
    readonly property color inkColor: backend.themeForeground
    readonly property color accentColor: backend.themeAccent
    readonly property color selectionColor: backend.themeSelection
    readonly property real textScale: backend.textScale

    function scaledSize(pixels) {
        return Math.max(1, Math.round(pixels * win.textScale));
    }

    Material.theme: darkMode ? Material.Dark : Material.Light
    Material.accent: accentColor
    color: pageColor

    onClosing: function(close) {
        // Closing the window is just "put the note away": the instance stays
        // resident so the next summon is instant. Ctrl+Q really quits.
        close.accepted = false;
        backend.hide();
    }

    Connections {
        target: backend
        function onSummoned() {
            editor.forceActiveFocus();
            editor.cursorPosition = editor.length;
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: backend.hide()
    }
    Shortcut {
        sequences: [StandardKey.Quit, "Ctrl+Q"]
        onActivated: backend.quit()
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        TextArea {
            id: editor

            // Loading the saved note also fires onTextChanged; only edits
            // made after that should be scheduled for saving.
            property bool restored: false

            wrapMode: TextArea.Wrap
            textFormat: TextEdit.PlainText
            placeholderText: "اكتبها قبل أن تختفي — jot it before it's gone"
            placeholderTextColor: Qt.alpha(win.inkColor, 0.35)
            color: win.inkColor
            selectionColor: win.selectionColor
            selectedTextColor: win.pageColor
            font.family: fixedFontFamily
            font.pixelSize: win.scaledSize(16)
            // The Material style binds top/bottom padding itself (for its
            // floating placeholder), so the shorthand alone doesn't reach them.
            padding: win.scaledSize(28)
            topPadding: win.scaledSize(28)
            bottomPadding: win.scaledSize(40)
            background: null

            Component.onCompleted: {
                text = backend.loadNote();
                restored = true;
                cursorPosition = length;
                forceActiveFocus();
            }
            onTextChanged: {
                if (restored)
                    backend.noteEdited(text);
            }
        }
    }

    // Save indicator: nothing when saved — a quiet page is the signal. While
    // a save is pending, a small soft dot breathes in the corner, then fades.
    Rectangle {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: win.scaledSize(14)
        width: 6
        height: 6
        radius: 3
        color: Qt.alpha(win.inkColor, 0.35)
        opacity: backend.dirty ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }
}
