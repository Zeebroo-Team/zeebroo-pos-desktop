import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RibbonUI

RibbonWindow {
    id: root
    width: 1280
    height: 800
    title: qsTr("Zeebroo POS")

    // ── Ribbon tab bar ─────────────────────────────────────────────────────
    RibbonTabBar {
        id: ribbon
        modernStyle: true
        onSettingsBtnClicked: settingsView.open()

        RibbonTabPage {
            title: qsTr("POS")
            RibbonTabGroup {
                text: qsTr("Session"); width: 150
                ColumnLayout { anchors.centerIn: parent; spacing: 2
                    RibbonButton { text: qsTr("New Session");   iconSource: RibbonIcons.Add }
                    RibbonButton { text: qsTr("Close Session"); iconSource: RibbonIcons.Dismiss }
                }
            }
            RibbonTabGroup {
                text: qsTr("Sales"); width: 190
                ColumnLayout { anchors.centerIn: parent; spacing: 2
                    RibbonButton { text: qsTr("Checkout  F12");     iconSource: RibbonIcons.Payment }
                    RibbonButton { text: qsTr("Return/Refund  F9"); iconSource: RibbonIcons.ArrowUndo }
                    RibbonButton { text: qsTr("Clear Cart  F8");    iconSource: RibbonIcons.Delete }
                }
            }
            RibbonTabGroup {
                text: qsTr("Find"); width: 200
                ColumnLayout { anchors.centerIn: parent; spacing: 2
                    RibbonButton { text: qsTr("Search Products  F2"); iconSource: RibbonIcons.Search }
                    RibbonButton { text: qsTr("Scan Barcode  F3");    iconSource: RibbonIcons.Scan }
                    RibbonButton { text: qsTr("Add Product  F4");     iconSource: RibbonIcons.AddSquare }
                }
            }
        }

        RibbonTabPage {
            title: qsTr("Inventory")
            RibbonTabGroup {
                text: qsTr("Catalog"); width: 175
                ColumnLayout { anchors.centerIn: parent; spacing: 2
                    RibbonButton { text: qsTr("Refresh  F5");       iconSource: RibbonIcons.ArrowClockwise }
                    RibbonButton { text: qsTr("Clear Filters  F6"); iconSource: RibbonIcons.Dismiss }
                }
            }
            RibbonTabGroup {
                text: qsTr("Purchase Orders"); width: 190
                ColumnLayout { anchors.centerIn: parent; spacing: 2
                    RibbonButton { text: qsTr("Orders  Ctrl+P"); iconSource: RibbonIcons.Document }
                    RibbonButton { text: qsTr("Add Supplier");   iconSource: RibbonIcons.Building }
                }
            }
            RibbonTabGroup {
                text: qsTr("Customers"); width: 145
                ColumnLayout { anchors.centerIn: parent; spacing: 2
                    RibbonButton { text: qsTr("Customers"); iconSource: RibbonIcons.People }
                    RibbonButton { text: qsTr("Accounts");  iconSource: RibbonIcons.PersonAccounts }
                }
            }
        }

        RibbonTabPage {
            title: qsTr("View")
            RibbonTabGroup {
                text: qsTr("Window"); width: 145
                ColumnLayout { anchors.centerIn: parent; spacing: 2
                    RibbonButton {
                        text: qsTr("Full Screen"); iconSource: RibbonIcons.FullScreenMaximize
                        checkable: true
                        onCheckedChanged: checked ? root.showFullScreen() : root.showNormal()
                    }
                }
            }
            RibbonTabGroup {
                text: qsTr("Appearance"); width: 175
                ColumnLayout { anchors.centerIn: parent; spacing: 8
                    RibbonSwitchButton {
                        text: qsTr("Dark Mode"); checked: RibbonTheme.isDarkMode
                        onCheckedChanged: RibbonTheme.themeMode =
                            checked ? RibbonThemeType.Dark : RibbonThemeType.Light
                    }
                    RibbonCheckBox {
                        text: qsTr("Modern Style"); iconFilledBgColor: "#4e8ef7"
                        checked: RibbonTheme.modernStyle
                        onCheckedChanged: RibbonTheme.modernStyle = checked
                    }
                }
            }
        }
    }

    // ── Content area ───────────────────────────────────────────────────────
    Rectangle {
        id: contentArea
        anchors { top: ribbon.bottom; left: parent.left; right: parent.right; bottom: statusBar.top }
        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#f0f2f5"

        // ── Tab state — plain booleans + active string ────────────────────
        property bool   flowchartTabOpen:  true
        property bool   billsTabOpen:      false
        property bool   createBillTabOpen: false
        property string activeTab:         "flowchart"   // "flowchart" | "bills" | "createBill" | ""

        // ── Zoom (flowchart only) ─────────────────────────────────────────
        property real zoomLevel: 1.0
        property real panX: 0
        property real panY: 0
        readonly property real minZoom: 0.3
        readonly property real maxZoom: 3.0
        function zoomIn()    { zoomLevel = Math.min(maxZoom, Math.round((zoomLevel + 0.15) * 100) / 100) }
        function zoomOut()   { zoomLevel = Math.max(minZoom, Math.round((zoomLevel - 0.15) * 100) / 100) }
        function resetZoom() { zoomLevel = 1.0; panX = 0; panY = 0 }
        Behavior on zoomLevel { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }

        // ── Tab strip ─────────────────────────────────────────────────────
        Rectangle {
            id: tabStrip
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 36
            color: RibbonTheme.isDarkMode ? "#13131f" : "#e2e4ef"

            Rectangle {
                anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                height: 1; color: RibbonTheme.isDarkMode ? "#2a2a42" : "#cdd0e0"
            }

            Row {
                anchors { left: parent.left; top: parent.top; bottom: parent.bottom; leftMargin: 6 }
                spacing: 2

                // ── Flowchart chip ─────────────────────────────────────────
                Item {
                    id: flowchartChip
                    visible: contentArea.flowchartTabOpen
                    property bool isActive: contentArea.activeTab === "flowchart"
                    width: visible ? (fcRow.implicitWidth + 20) : 0
                    height: tabStrip.height + 1
                    y: -1

                    Rectangle {
                        anchors.fill: parent; radius: 7
                        color: flowchartChip.isActive
                               ? (RibbonTheme.isDarkMode ? "#1e1e2e" : "#f0f2f5")
                               : (RibbonTheme.isDarkMode ? "#1a1a2e" : "#e8eaf2")
                        border.width: 1
                        border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#cdd0e0"
                        Rectangle {
                            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                            height: 8
                            color: flowchartChip.isActive
                                   ? (RibbonTheme.isDarkMode ? "#1e1e2e" : "#f0f2f5")
                                   : "transparent"
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: contentArea.activeTab = "flowchart"
                        }
                    }
                    RowLayout {
                        id: fcRow
                        anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; leftMargin: 10; rightMargin: 2 }
                        spacing: 5
                        RibbonIcon { iconSource: RibbonIcons.ChartMultiple; iconSize: 13; opacity: flowchartChip.isActive ? 0.85 : 0.5 }
                        RibbonText { text: qsTr("Flowchart"); font.pixelSize: 12; opacity: flowchartChip.isActive ? 1.0 : 0.6 }
                        RibbonButton {
                            showBg: false; adaptHeight: true; iconSource: RibbonIcons.Dismiss; tipText: qsTr("Close tab")
                            onClicked: {
                                contentArea.flowchartTabOpen = false
                                if (contentArea.activeTab === "flowchart")
                                    contentArea.activeTab = contentArea.billsTabOpen ? "bills" : contentArea.createBillTabOpen ? "createBill" : ""
                            }
                        }
                    }
                }

                // ── Create Bill chip ───────────────────────────────────────
                Item {
                    id: createBillChip
                    visible: contentArea.createBillTabOpen
                    property bool isActive: contentArea.activeTab === "createBill"
                    width: visible ? (cbRow.implicitWidth + 20) : 0
                    height: tabStrip.height + 1
                    y: -1

                    Rectangle {
                        anchors.fill: parent; radius: 7
                        color: createBillChip.isActive
                               ? (RibbonTheme.isDarkMode ? "#1e1e2e" : "#f0f2f5")
                               : (RibbonTheme.isDarkMode ? "#1a1a2e" : "#e8eaf2")
                        border.width: 1
                        border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#cdd0e0"
                        Rectangle {
                            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                            height: 8
                            color: createBillChip.isActive
                                   ? (RibbonTheme.isDarkMode ? "#1e1e2e" : "#f0f2f5")
                                   : "transparent"
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: contentArea.activeTab = "createBill"
                        }
                    }
                    RowLayout {
                        id: cbRow
                        anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; leftMargin: 10; rightMargin: 2 }
                        spacing: 5
                        RibbonIcon { iconSource: RibbonIcons.Document; iconSize: 13; opacity: createBillChip.isActive ? 0.85 : 0.5 }
                        RibbonText { text: qsTr("Create Bill"); font.pixelSize: 12; opacity: createBillChip.isActive ? 1.0 : 0.6 }
                        RibbonButton {
                            showBg: false; adaptHeight: true; iconSource: RibbonIcons.Dismiss; tipText: qsTr("Close tab")
                            onClicked: {
                                contentArea.createBillTabOpen = false
                                if (contentArea.activeTab === "createBill")
                                    contentArea.activeTab = contentArea.flowchartTabOpen ? "flowchart" : ""
                            }
                        }
                    }
                }

                // ── Bills chip ─────────────────────────────────────────────
                Item {
                    id: billsChip
                    visible: contentArea.billsTabOpen
                    property bool isActive: contentArea.activeTab === "bills"
                    width: visible ? (blRow.implicitWidth + 20) : 0
                    height: tabStrip.height + 1
                    y: -1

                    Rectangle {
                        anchors.fill: parent; radius: 7
                        color: billsChip.isActive
                               ? (RibbonTheme.isDarkMode ? "#1e1e2e" : "#f0f2f5")
                               : (RibbonTheme.isDarkMode ? "#1a1a2e" : "#e8eaf2")
                        border.width: 1
                        border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#cdd0e0"
                        Rectangle {
                            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                            height: 8
                            color: billsChip.isActive
                                   ? (RibbonTheme.isDarkMode ? "#1e1e2e" : "#f0f2f5")
                                   : "transparent"
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: contentArea.activeTab = "bills"
                        }
                    }
                    RowLayout {
                        id: blRow
                        anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; leftMargin: 10; rightMargin: 2 }
                        spacing: 5
                        RibbonIcon { iconSource: RibbonIcons.Document; iconSize: 13; color: "#4e8ef7"; opacity: billsChip.isActive ? 1.0 : 0.5 }
                        RibbonText { text: qsTr("Bills"); font.pixelSize: 12; opacity: billsChip.isActive ? 1.0 : 0.6 }
                        RibbonButton {
                            showBg: false; adaptHeight: true; iconSource: RibbonIcons.Dismiss; tipText: qsTr("Close tab")
                            onClicked: {
                                contentArea.billsTabOpen = false
                                if (contentArea.activeTab === "bills")
                                    contentArea.activeTab = contentArea.flowchartTabOpen ? "flowchart" : contentArea.createBillTabOpen ? "createBill" : ""
                            }
                        }
                    }
                }

                // "Reopen" button when all tabs closed
                RibbonButton {
                    visible: !contentArea.flowchartTabOpen && !contentArea.billsTabOpen && !contentArea.createBillTabOpen
                    anchors.verticalCenter: parent.verticalCenter
                    showBg: false; adaptHeight: true; iconSource: RibbonIcons.Add
                    text: qsTr("Flowchart"); tipText: qsTr("Reopen Flowchart panel")
                    onClicked: { contentArea.flowchartTabOpen = true; contentArea.activeTab = "flowchart" }
                }
            }
        }

        // ── Panel body ────────────────────────────────────────────────────
        Item {
            id: panelArea
            clip: true
            anchors { top: tabStrip.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }

            Canvas {
                id: gridCanvas
                anchors.fill: parent
                visible: contentArea.activeTab === "flowchart"
                Connections { target: RibbonTheme; function onIsDarkModeChanged() { gridCanvas.requestPaint() } }
                onWidthChanged:  requestPaint()
                onHeightChanged: requestPaint()
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    ctx.fillStyle = RibbonTheme.isDarkMode ? "rgba(255,255,255,0.07)" : "rgba(0,0,0,0.10)"
                    var sp = 28
                    for (var x = sp / 2; x < width; x += sp)
                        for (var y = sp / 2; y < height; y += sp) {
                            ctx.beginPath(); ctx.arc(x, y, 1.4, 0, Math.PI * 2); ctx.fill()
                        }
                }
            }

            // ── Empty state ───────────────────────────────────────────────
            Column {
                anchors.centerIn: parent; spacing: 14
                visible: !contentArea.flowchartTabOpen && !contentArea.createBillTabOpen
                RibbonIcon { anchors.horizontalCenter: parent.horizontalCenter; iconSource: RibbonIcons.Add; iconSize: 40; opacity: 0.18 }
                RibbonText { anchors.horizontalCenter: parent.horizontalCenter; text: qsTr("No panels open"); font.pixelSize: 14; opacity: 0.28 }
                RibbonButton {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Open Flowchart"); iconSource: RibbonIcons.Add
                    onClicked: { contentArea.flowchartTabOpen = true; contentArea.activeTab = "flowchart" }
                }
            }

            // ════════════════════════════════════════════════════════════
            // FLOWCHART TAB
            // ════════════════════════════════════════════════════════════
            MouseArea {
                id: chartPanArea
                anchors.fill: parent
                enabled: contentArea.flowchartTabOpen && contentArea.activeTab === "flowchart"
                propagateComposedEvents: true
                cursorShape: pressed && _panning ? Qt.ClosedHandCursor : Qt.OpenHandCursor

                property real _sx: 0; property real _sy: 0
                property bool _panning: false

                onWheel: function(wheel) {
                    if (wheel.angleDelta.y > 0) contentArea.zoomIn()
                    else                        contentArea.zoomOut()
                }
                onPressed: function(mouse) {
                    _sx = mouse.x; _sy = mouse.y; _panning = false
                }
                onPositionChanged: function(mouse) {
                    var dx = mouse.x - _sx, dy = mouse.y - _sy
                    if (!_panning && (dx * dx + dy * dy) > 16) _panning = true
                    if (_panning) {
                        contentArea.panX += dx; contentArea.panY += dy
                        _sx = mouse.x; _sy = mouse.y
                    }
                }
                onReleased: function(mouse) { _panning = false }
                onClicked:  function(mouse) { mouse.accepted = false }
                onDoubleClicked: function(mouse) { contentArea.resetZoom() }
            }

            Item {
                id: flowChart
                visible: contentArea.flowchartTabOpen && contentArea.activeTab === "flowchart"
                width:  panelArea.width
                height: panelArea.height
                x: panelArea.width  * (1 - contentArea.zoomLevel) / 2 + contentArea.panX
                y: panelArea.height * (1 - contentArea.zoomLevel) / 2 + contentArea.panY
                scale: contentArea.zoomLevel
                transformOrigin: Item.TopLeft

                property real cx: width  / 2
                property real cy: height / 2
                property real r1: Math.min(width, height) * 0.30

                // Expenses node sits at -150° in the main ring
                property real expNodeX: cx + r1 * Math.cos(-150 * Math.PI / 180)
                property real expNodeY: cy + r1 * Math.sin(-150 * Math.PI / 180)

                // Sub-node card dimensions
                readonly property real subCellW: 118
                readonly property real subCellH: 36
                // Radial distance from Expenses center to sub-node centers
                // 0.45 * min ensures chord at 24° step = ~131px > subCellW, so no overlap
                property real r2: Math.min(width, height) * 0.45
                // Leaf node radial parameters
                readonly property real lnLeafGap:  88
                readonly property real lnLeafStep: 54

                property var mainNodes: [
                    { idx: 0, label: qsTr("POS"),       icon: RibbonIcons.Cart,          accent: "#4caf7d", angle: -90,  action: "openPos" },
                    { idx: 1, label: qsTr("Inventory"), icon: RibbonIcons.Box,           accent: "#4e8ef7", angle: -30,  action: "" },
                    { idx: 2, label: qsTr("Customers"), icon: RibbonIcons.People,        accent: "#9c6ef7", angle:  30,  action: "" },
                    { idx: 3, label: qsTr("Reports"),   icon: RibbonIcons.ChartMultiple, accent: "#f7a54e", angle:  90,  action: "" },
                    { idx: 4, label: qsTr("Income"),    icon: RibbonIcons.MoneyHand,     accent: "#60c060", angle:  150, action: "" },
                    { idx: 5, label: qsTr("Expenses"),  icon: RibbonIcons.MoneyDismiss,  accent: "#f74e6c", angle: -150, action: "" },
                ]

                // idx indexes into subNodeCenters; angle (deg, QML y-down) sets the radial initial position
                // Fan at 24° steps from 130°→322° keeps chord > subCellW and stays clear of hub/POS
                property var expenseSubNodes: [
                    { idx: 0, angle: 130, label: qsTr("Bill"),             icon: RibbonIcons.Document,        accent: "#4e8ef7", action: "openBill" },
                    { idx: 1, angle: 154, label: qsTr("Loan Setup"),       icon: RibbonIcons.MoneyCalculator, accent: "#9c6ef7", action: "" },
                    { idx: 2, angle: 178, label: qsTr("Rental"),           icon: RibbonIcons.Home,            accent: "#f7a54e", action: "" },
                    { idx: 3, angle: 202, label: qsTr("Employee Salary"),  icon: RibbonIcons.PersonMoney,     accent: "#4caf7d", action: "" },
                    { idx: 4, angle: 226, label: qsTr("Modification"),     icon: RibbonIcons.Wrench,          accent: "#f0a030", action: "" },
                    { idx: 5, angle: 250, label: qsTr("Purchase Order"),   icon: RibbonIcons.ShoppingBag,     accent: "#4ee0f7", action: "" },
                    { idx: 6, angle: 274, label: qsTr("Legal"),            icon: RibbonIcons.Scales,          accent: "#f74e6c", action: "" },
                    { idx: 7, angle: 298, label: qsTr("Marketing"),        icon: RibbonIcons.Megaphone,       accent: "#e040fb", action: "" },
                    { idx: 8, angle: 322, label: qsTr("Transport"),        icon: RibbonIcons.Airplane,        accent: "#60c060", action: "" },
                ]

                // Delegates write here on every move; canvas reads these for line endpoints
                property var mainNodeCenters: [Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0)]
                property var subNodeCenters:  [Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0)]

                // Live data nodes fetched from API — smaller than sub-nodes
                readonly property real lnW: 96
                readonly property real lnH: 24

                property var loanNodes:     []
                property var billNodes:     []
                property var rentalNodes:   []
                property var employeeNodes: []
                property var modNodes:      []

                // Actual center positions of leaf nodes (updated on every move)
                property var billLeafCenters:     []
                property var loanLeafCenters:     []
                property var rentalLeafCenters:   []
                property var employeeLeafCenters: []
                property var modLeafCenters:      []

                Connections {
                    target: appController
                    function onLoansLoaded(loans)         { flowChart.loanNodes    = loans;     lineCanvas.requestPaint() }
                    function onBillsLoaded(bills)         { flowChart.billNodes    = bills;     lineCanvas.requestPaint() }
                    function onRentalsLoaded(rentals)     { flowChart.rentalNodes  = rentals;   lineCanvas.requestPaint() }
                    function onModificationsLoaded(mods)  { flowChart.modNodes     = mods;      lineCanvas.requestPaint() }
                    function onEmployeesLoaded(emps)      { flowChart.employeeNodes= emps;      lineCanvas.requestPaint() }
                }
                Component.onCompleted: Qt.callLater(function() {
                    appController.fetchLoans()
                    appController.fetchBills()
                    appController.fetchRentals()
                    appController.fetchModifications()
                    appController.fetchEmployees()
                })

                // Push `item` out of all overlapping nodes. Call on drag release.
                // selfIdx / isSub identify which node is being moved (skip self-check).
                function separateNode(item, selfIdx, isSub) {
                    var ix = item.x, iy = item.y
                    var hw = item.width / 2, hh = item.height / 2
                    var gap = 10

                    // Axis-aligned push: if item overlaps rect centred at (ocx,ocy) with half-sizes (ohw,ohh), push item away
                    function push(ocx, ocy, ohw, ohh) {
                        if (Math.abs(ix + hw - ocx) >= hw + ohw) return
                        if (Math.abs(iy + hh - ocy) >= hh + ohh) return
                        var dx = ix + hw - ocx, dy = iy + hh - ocy
                        var mag = Math.sqrt(dx * dx + dy * dy)
                        if (mag < 0.5) { dx = 1; dy = 0; mag = 1 }
                        var p = hw + ohw + gap - mag
                        if (p > 0) { ix += dx / mag * p; iy += dy / mag * p }
                    }

                    for (var pass = 0; pass < 3; pass++) {
                        push(cx, cy, 90, 31)                              // center hub
                        for (var i = 0; i < mainNodeCenters.length; i++) {
                            if (!isSub && i === selfIdx) continue
                            var mc = mainNodeCenters[i]
                            if (mc.x !== 0 || mc.y !== 0) push(mc.x, mc.y, 69, 26)
                        }
                        for (var j = 0; j < subNodeCenters.length; j++) {
                            if (isSub && j === selfIdx) continue
                            var sc = subNodeCenters[j]
                            if (sc.x !== 0 || sc.y !== 0) push(sc.x, sc.y, subCellW / 2, subCellH / 2)
                        }
                    }

                    item.x = ix
                    item.y = iy
                }

                onWidthChanged:  lineCanvas.requestPaint()
                onHeightChanged: lineCanvas.requestPaint()

                Canvas {
                    id: lineCanvas
                    anchors.fill: parent
                    Connections { target: RibbonTheme; function onIsDarkModeChanged() { lineCanvas.requestPaint() } }
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        var lineCol  = RibbonTheme.isDarkMode ? "#4a5a8a" : "#9aaacf"
                        var hubR = 56, nodeR = 22
                        var mainNodes    = flowChart.mainNodes
                        var expSubNodes  = flowChart.expenseSubNodes

                        // Hub → each main node — solid colored lines with filled arrow
                        var mCenters = flowChart.mainNodeCenters
                        for (var i = 0; i < mCenters.length; i++) {
                            var nc = mCenters[i]
                            if (nc.x === 0 && nc.y === 0) continue
                            var ddx = nc.x - flowChart.cx, ddy = nc.y - flowChart.cy
                            var dist = Math.sqrt(ddx * ddx + ddy * ddy)
                            if (dist < 1) continue
                            var nnx = ddx / dist, nny = ddy / dist
                            var x1 = flowChart.cx + nnx * hubR
                            var y1 = flowChart.cy + nny * hubR
                            var x2 = nc.x - nnx * nodeR
                            var y2 = nc.y - nny * nodeR
                            var accent = (mainNodes && mainNodes[i]) ? mainNodes[i].accent : lineCol
                            ctx.strokeStyle = accent; ctx.lineWidth = 2.5; ctx.setLineDash([])
                            ctx.beginPath(); ctx.moveTo(x1, y1); ctx.lineTo(x2, y2); ctx.stroke()
                            ctx.fillStyle = accent
                            var sz = 9, aa = Math.atan2(y2 - y1, x2 - x1)
                            ctx.beginPath()
                            ctx.moveTo(x2, y2)
                            ctx.lineTo(x2 - sz * Math.cos(aa - 0.38), y2 - sz * Math.sin(aa - 0.38))
                            ctx.lineTo(x2 - sz * Math.cos(aa + 0.38), y2 - sz * Math.sin(aa + 0.38))
                            ctx.closePath(); ctx.fill()
                        }

                        // Expenses node → each expense sub-node — colored dashed lines
                        var expC = mCenters[5]
                        var sCenters = flowChart.subNodeCenters
                        if (expC && (expC.x !== 0 || expC.y !== 0)) {
                            for (var j = 0; j < sCenters.length; j++) {
                                var sc = sCenters[j]
                                if (sc.x === 0 && sc.y === 0) continue
                                var sdx = sc.x - expC.x, sdy = sc.y - expC.y
                                var sdist = Math.sqrt(sdx * sdx + sdy * sdy)
                                if (sdist < 1) continue
                                var snx = sdx / sdist, sny = sdy / sdist
                                var subAccent = (expSubNodes && expSubNodes[j]) ? expSubNodes[j].accent : lineCol
                                ctx.strokeStyle = subAccent; ctx.lineWidth = 1.5; ctx.setLineDash([6, 4])
                                ctx.beginPath()
                                ctx.moveTo(expC.x + snx * nodeR, expC.y + sny * nodeR)
                                ctx.lineTo(sc.x - snx * 20,      sc.y - sny * 20)
                                ctx.stroke()
                                ctx.setLineDash([])
                            }
                        }

                        // Draw lines from sub-node center to each leaf node's ACTUAL center
                        function drawLeafLines(subCenter, leafCenters, color) {
                            if (!leafCenters || leafCenters.length === 0) return
                            if (!subCenter || (subCenter.x === 0 && subCenter.y === 0)) return
                            for (var ki = 0; ki < leafCenters.length; ki++) {
                                var lc = leafCenters[ki]
                                if (!lc || (lc.x === 0 && lc.y === 0)) continue
                                var dx = lc.x - subCenter.x, dy = lc.y - subCenter.y
                                var dlen = Math.sqrt(dx * dx + dy * dy)
                                if (dlen < 2) continue
                                var nx = dx / dlen, ny = dy / dlen
                                ctx.strokeStyle = color; ctx.lineWidth = 1.5; ctx.setLineDash([5, 4])
                                ctx.beginPath()
                                ctx.moveTo(subCenter.x + nx * 20, subCenter.y + ny * 20)
                                ctx.lineTo(lc.x - nx * 14,        lc.y - ny * 14)
                                ctx.stroke()
                                ctx.setLineDash([])
                            }
                        }
                        drawLeafLines(flowChart.subNodeCenters[0], flowChart.billLeafCenters,     "#4e8ef7")
                        drawLeafLines(flowChart.subNodeCenters[1], flowChart.loanLeafCenters,     "#9c6ef7")
                        drawLeafLines(flowChart.subNodeCenters[2], flowChart.rentalLeafCenters,   "#f7a54e")
                        drawLeafLines(flowChart.subNodeCenters[3], flowChart.employeeLeafCenters, "#4caf7d")
                        drawLeafLines(flowChart.subNodeCenters[4], flowChart.modLeafCenters,      "#f0a030")
                    }
                }

                // Center hub — business name
                Rectangle {
                    x: flowChart.cx - width / 2; y: flowChart.cy - height / 2
                    width: 180; height: 62; radius: 31; color: "#4e8ef7"; z: 2
                    layer.enabled: true; layer.effect: RibbonShadow {}
                    RowLayout {
                        anchors.centerIn: parent; spacing: 8
                        RibbonIcon { iconSource: RibbonIcons.BuildingRetail; iconSize: 18; color: "white" }
                        RibbonText {
                            text: appController.businessName !== "" ? appController.businessName : qsTr("Business")
                            color: "white"; font.pixelSize: 13; font.bold: true
                        }
                    }
                }

                // Main ring — 6 draggable nodes
                Repeater {
                    model: flowChart.mainNodes
                    delegate: Item {
                        id: mainNode
                        required property var modelData
                        width: 138; height: 52
                        z: nodeMA._dragging ? 4 : 1
                        property bool _moved: false
                        property bool _ready: false

                        function _initPos() {
                            x = flowChart.cx + flowChart.r1 * Math.cos(modelData.angle * Math.PI / 180) - width  / 2
                            y = flowChart.cy + flowChart.r1 * Math.sin(modelData.angle * Math.PI / 180) - height / 2
                        }
                        Component.onCompleted: {
                            _initPos()
                            Qt.callLater(function() { mainNode._ready = true })
                        }
                        Connections {
                            target: flowChart
                            function onCxChanged() { if (!mainNode._moved) mainNode._initPos() }
                            function onCyChanged() { if (!mainNode._moved) mainNode._initPos() }
                            function onR1Changed() { if (!mainNode._moved) mainNode._initPos() }
                        }

                        Behavior on x { enabled: mainNode._ready && !nodeMA._dragging; NumberAnimation { duration: 160; easing.type: Easing.OutCubic } }
                        Behavior on y { enabled: mainNode._ready && !nodeMA._dragging; NumberAnimation { duration: 160; easing.type: Easing.OutCubic } }

                        onXChanged: {
                            var c = flowChart.mainNodeCenters.slice()
                            c[modelData.idx] = Qt.point(x + width / 2, y + height / 2)
                            flowChart.mainNodeCenters = c
                            lineCanvas.requestPaint()
                        }
                        onYChanged: {
                            var c = flowChart.mainNodeCenters.slice()
                            c[modelData.idx] = Qt.point(x + width / 2, y + height / 2)
                            flowChart.mainNodeCenters = c
                            lineCanvas.requestPaint()
                        }

                        Rectangle {
                            id: nodeRect
                            anchors.fill: parent; radius: 10
                            color: RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            border.width: 1.5; border.color: modelData.accent
                            layer.enabled: true; layer.effect: RibbonShadow {}
                            RowLayout {
                                anchors { fill: parent; margins: 9 }
                                spacing: 8
                                Rectangle {
                                    width: 30; height: 30; radius: 7; color: modelData.accent
                                    RibbonIcon { anchors.centerIn: parent; iconSource: modelData.icon; iconSize: 15; color: "white" }
                                }
                                RibbonText { text: modelData.label; font.pixelSize: 12; font.bold: true }
                            }
                        }

                        MouseArea {
                            id: nodeMA
                            anchors.fill: parent
                            hoverEnabled: true
                            property bool _dragging: false
                            property bool _didDrag:  false
                            property real _pgx: 0;  property real _pgy: 0
                            cursorShape: _dragging ? Qt.ClosedHandCursor : Qt.PointingHandCursor

                            onPressed: function(mouse) {
                                var g = mapToItem(flowChart, mouse.x, mouse.y)
                                _pgx = g.x; _pgy = g.y
                                _dragging = true; _didDrag = false
                                mainNode._moved = true
                            }
                            onPositionChanged: function(mouse) {
                                if (!_dragging) return
                                var g  = mapToItem(flowChart, mouse.x, mouse.y)
                                var dx = g.x - _pgx, dy = g.y - _pgy
                                _pgx = g.x; _pgy = g.y
                                if (Math.abs(dx) + Math.abs(dy) > 0.5) _didDrag = true
                                mainNode.x += dx; mainNode.y += dy
                                flowChart.separateNode(mainNode, modelData.idx, false)
                            }
                            onReleased: function(mouse) {
                                _dragging = false
                                nodeRect.color = RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            }
                            onEntered: nodeRect.color = RibbonTheme.isDarkMode ? "#303045" : "#eef2ff"
                            onExited:  nodeRect.color = RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            onClicked: { if (!_didDrag && modelData.action === "openPos") appController.openPos() }
                        }
                    }
                }

                // Expense sub-nodes — 3×3 grid, each individually draggable
                Repeater {
                    model: flowChart.expenseSubNodes
                    delegate: Item {
                        id: subNode
                        required property var modelData
                        width: flowChart.subCellW; height: flowChart.subCellH
                        z: subMA._dragging ? 4 : 1
                        property bool _moved: false
                        property bool _ready: false

                        function _initPos() {
                            var a = modelData.angle * Math.PI / 180
                            x = flowChart.expNodeX + flowChart.r2 * Math.cos(a) - width  / 2
                            y = flowChart.expNodeY + flowChart.r2 * Math.sin(a) - height / 2
                        }
                        Component.onCompleted: {
                            _initPos()
                            Qt.callLater(function() { subNode._ready = true })
                        }
                        Connections {
                            target: flowChart
                            function onExpNodeXChanged() { if (!subNode._moved) subNode._initPos() }
                            function onExpNodeYChanged() { if (!subNode._moved) subNode._initPos() }
                        }

                        Behavior on x { enabled: subNode._ready && !subMA._dragging; NumberAnimation { duration: 160; easing.type: Easing.OutCubic } }
                        Behavior on y { enabled: subNode._ready && !subMA._dragging; NumberAnimation { duration: 160; easing.type: Easing.OutCubic } }

                        onXChanged: {
                            var c = flowChart.subNodeCenters.slice()
                            c[modelData.idx] = Qt.point(x + width / 2, y + height / 2)
                            flowChart.subNodeCenters = c
                            lineCanvas.requestPaint()
                        }
                        onYChanged: {
                            var c = flowChart.subNodeCenters.slice()
                            c[modelData.idx] = Qt.point(x + width / 2, y + height / 2)
                            flowChart.subNodeCenters = c
                            lineCanvas.requestPaint()
                        }

                        Rectangle {
                            id: subRect
                            anchors.fill: parent; radius: 7
                            color: RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            border.width: 1.5; border.color: modelData.accent
                            layer.enabled: true; layer.effect: RibbonShadow {}
                            RowLayout {
                                anchors { fill: parent; leftMargin: 7; rightMargin: 5; topMargin: 3; bottomMargin: 3 }
                                spacing: 6
                                Rectangle {
                                    width: 22; height: 22; radius: 5; color: modelData.accent
                                    RibbonIcon { anchors.centerIn: parent; iconSource: modelData.icon; iconSize: 11; color: "white" }
                                }
                                RibbonText { text: modelData.label; font.pixelSize: 10; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                            }
                        }

                        MouseArea {
                            id: subMA
                            anchors.fill: parent
                            hoverEnabled: true
                            property bool _dragging: false
                            property bool _didDrag:  false
                            property real _pgx: 0;  property real _pgy: 0
                            cursorShape: _dragging ? Qt.ClosedHandCursor : Qt.PointingHandCursor

                            onPressed: function(mouse) {
                                var g = mapToItem(flowChart, mouse.x, mouse.y)
                                _pgx = g.x; _pgy = g.y
                                _dragging = true; _didDrag = false
                                subNode._moved = true
                            }
                            onPositionChanged: function(mouse) {
                                if (!_dragging) return
                                var g  = mapToItem(flowChart, mouse.x, mouse.y)
                                var dx = g.x - _pgx, dy = g.y - _pgy
                                _pgx = g.x; _pgy = g.y
                                if (Math.abs(dx) + Math.abs(dy) > 0.5) _didDrag = true
                                subNode.x += dx; subNode.y += dy
                                flowChart.separateNode(subNode, modelData.idx, true)
                            }
                            onReleased: function(mouse) {
                                _dragging = false
                                subRect.color = RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            }
                            onEntered: subRect.color = RibbonTheme.isDarkMode ? "#303045" : "#eef2ff"
                            onExited:  subRect.color = RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            onClicked: {
                                if (!_didDrag && modelData.action === "openBill") {
                                    contentArea.billsTabOpen = true
                                    contentArea.activeTab = "bills"
                                }
                            }
                        }
                    }
                }

                // ── Data leaf node component ──────────────────────────────
                component DataLeafNode: Item {
                    id: leafRoot
                    required property var    modelData
                    required property int    index
                    required property real   nodeX
                    required property real   nodeY
                    required property string accent
                    required property var    iconSrc
                    required property bool   isOverdue

                    width: flowChart.lnW; height: flowChart.lnH
                    z: leafMA._dragging ? 5 : 3

                    property bool _moved: false
                    property bool _ready: false

                    // Follow computed position until user drags
                    onNodeXChanged: if (!_moved) x = nodeX
                    onNodeYChanged: if (!_moved) y = nodeY

                    Component.onCompleted: {
                        x = nodeX; y = nodeY
                        Qt.callLater(function() { leafRoot._ready = true })
                    }

                    Behavior on x { enabled: leafRoot._ready && !leafMA._dragging; NumberAnimation { duration: 160; easing.type: Easing.OutCubic } }
                    Behavior on y { enabled: leafRoot._ready && !leafMA._dragging; NumberAnimation { duration: 160; easing.type: Easing.OutCubic } }

                    onXChanged: lineCanvas.requestPaint()
                    onYChanged: lineCanvas.requestPaint()

                    Rectangle {
                        id: leafRect
                        anchors.fill: parent; radius: 12
                        color: leafRoot.isOverdue ? "#ef4444" : leafRoot.accent
                        layer.enabled: true; layer.effect: RibbonShadow {}
                        RowLayout {
                            anchors { fill: parent; leftMargin: 6; rightMargin: 6; topMargin: 3; bottomMargin: 3 }
                            spacing: 4
                            RibbonIcon { iconSource: leafRoot.iconSrc; iconSize: 9; color: "white"; opacity: 0.9 }
                            Text { text: leafRoot.modelData.name; font.pixelSize: 9; font.bold: true; color: "white"; elide: Text.ElideRight; Layout.fillWidth: true }
                        }
                    }

                    MouseArea {
                        id: leafMA
                        anchors.fill: parent
                        hoverEnabled: true
                        property bool _dragging: false
                        property real _pgx: 0; property real _pgy: 0
                        cursorShape: _dragging ? Qt.ClosedHandCursor : Qt.PointingHandCursor

                        onPressed: function(mouse) {
                            var g = mapToItem(flowChart, mouse.x, mouse.y)
                            _pgx = g.x; _pgy = g.y
                            _dragging = true
                            leafRoot._moved = true
                        }
                        onPositionChanged: function(mouse) {
                            if (!_dragging) return
                            var g  = mapToItem(flowChart, mouse.x, mouse.y)
                            var dx = g.x - _pgx, dy = g.y - _pgy
                            _pgx = g.x; _pgy = g.y
                            leafRoot.x += dx; leafRoot.y += dy
                        }
                        onReleased: function(mouse) { _dragging = false }
                        onEntered:  leafRect.opacity = 0.85
                        onExited:   leafRect.opacity = 1.0
                    }
                }

                // Bill leaf nodes — radially outward at 130° from Expenses
                Repeater {
                    model: flowChart.billNodes
                    delegate: DataLeafNode {
                        property real _a: 130 * Math.PI / 180
                        property real _d: flowChart.r2 + flowChart.lnLeafGap + index * flowChart.lnLeafStep
                        nodeX: flowChart.expNodeX + _d * Math.cos(_a) - flowChart.lnW / 2
                        nodeY: flowChart.expNodeY + _d * Math.sin(_a) - flowChart.lnH / 2
                        accent: "#4e8ef7"; iconSrc: RibbonIcons.Document
                        isOverdue: modelData.overdue === true
                        onXChanged: { var c = flowChart.billLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.billLeafCenters = c }
                        onYChanged: { var c = flowChart.billLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.billLeafCenters = c }
                    }
                }

                // Loan leaf nodes — radially outward at 154° from Expenses
                Repeater {
                    model: flowChart.loanNodes
                    delegate: DataLeafNode {
                        property real _a: 154 * Math.PI / 180
                        property real _d: flowChart.r2 + flowChart.lnLeafGap + index * flowChart.lnLeafStep
                        nodeX: flowChart.expNodeX + _d * Math.cos(_a) - flowChart.lnW / 2
                        nodeY: flowChart.expNodeY + _d * Math.sin(_a) - flowChart.lnH / 2
                        accent: "#9c6ef7"; iconSrc: RibbonIcons.MoneyCalculator; isOverdue: false
                        onXChanged: { var c = flowChart.loanLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.loanLeafCenters = c }
                        onYChanged: { var c = flowChart.loanLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.loanLeafCenters = c }
                    }
                }

                // Rental leaf nodes — radially outward at 178° from Expenses
                Repeater {
                    model: flowChart.rentalNodes
                    delegate: DataLeafNode {
                        property real _a: 178 * Math.PI / 180
                        property real _d: flowChart.r2 + flowChart.lnLeafGap + index * flowChart.lnLeafStep
                        nodeX: flowChart.expNodeX + _d * Math.cos(_a) - flowChart.lnW / 2
                        nodeY: flowChart.expNodeY + _d * Math.sin(_a) - flowChart.lnH / 2
                        accent: "#f7a54e"; iconSrc: RibbonIcons.Home; isOverdue: false
                        onXChanged: { var c = flowChart.rentalLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.rentalLeafCenters = c }
                        onYChanged: { var c = flowChart.rentalLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.rentalLeafCenters = c }
                    }
                }

                // Employee leaf nodes — radially outward at 202° from Expenses
                Repeater {
                    model: flowChart.employeeNodes
                    delegate: DataLeafNode {
                        property real _a: 202 * Math.PI / 180
                        property real _d: flowChart.r2 + flowChart.lnLeafGap + index * flowChart.lnLeafStep
                        nodeX: flowChart.expNodeX + _d * Math.cos(_a) - flowChart.lnW / 2
                        nodeY: flowChart.expNodeY + _d * Math.sin(_a) - flowChart.lnH / 2
                        accent: "#4caf7d"; iconSrc: RibbonIcons.People; isOverdue: false
                        onXChanged: { var c = flowChart.employeeLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.employeeLeafCenters = c }
                        onYChanged: { var c = flowChart.employeeLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.employeeLeafCenters = c }
                    }
                }

                // Modification leaf nodes — radially outward at 226° from Expenses
                Repeater {
                    model: flowChart.modNodes
                    delegate: DataLeafNode {
                        property real _a: 226 * Math.PI / 180
                        property real _d: flowChart.r2 + flowChart.lnLeafGap + index * flowChart.lnLeafStep
                        nodeX: flowChart.expNodeX + _d * Math.cos(_a) - flowChart.lnW / 2
                        nodeY: flowChart.expNodeY + _d * Math.sin(_a) - flowChart.lnH / 2
                        accent: "#f0a030"; iconSrc: RibbonIcons.Wrench; isOverdue: false
                        onXChanged: { var c = flowChart.modLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.modLeafCenters = c }
                        onYChanged: { var c = flowChart.modLeafCenters.slice(); c[index] = Qt.point(x + width/2, y + height/2); flowChart.modLeafCenters = c }
                    }
                }
            }

            Rectangle {
                id: zoomPanel
                anchors { right: parent.right; bottom: parent.bottom; margins: 16 }
                width: 220
                height: zoomPanelVisible ? zpHeader.height + zpControls.height : 0
                visible: zoomPanelVisible && contentArea.flowchartTabOpen && contentArea.activeTab === "flowchart"
                radius: 10
                color: RibbonTheme.isDarkMode ? "#1c1c2e" : "#ffffff"
                border.width: 1; border.color: RibbonTheme.isDarkMode ? "#3a3a5a" : "#d8ddef"
                layer.enabled: true; layer.effect: RibbonShadow {}
                clip: true
                property bool zoomPanelVisible: true
                Behavior on height { NumberAnimation { duration: 160; easing.type: Easing.OutCubic } }

                Rectangle {
                    id: zpHeader
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    height: 30; radius: 10
                    color: RibbonTheme.isDarkMode ? "#2a2a42" : "#f0f2fa"
                    Rectangle {
                        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                        height: parent.radius; color: parent.color
                    }
                    RowLayout {
                        anchors { fill: parent; leftMargin: 10; rightMargin: 4 }
                        spacing: 6
                        RibbonIcon { iconSource: RibbonIcons.ZoomIn; iconSize: 13; opacity: 0.7 }
                        RibbonText { text: qsTr("View Controls"); font.pixelSize: 11; font.bold: true; opacity: 0.8; Layout.fillWidth: true }
                        RibbonButton { showBg: false; adaptHeight: true; iconSource: RibbonIcons.Dismiss; tipText: qsTr("Close panel"); onClicked: zoomPanel.zoomPanelVisible = false }
                    }
                }
                Rectangle {
                    id: zpControls
                    anchors { top: zpHeader.bottom; left: parent.left; right: parent.right }
                    height: 44; color: "transparent"
                    RowLayout {
                        anchors { fill: parent; leftMargin: 6; rightMargin: 6 }
                        spacing: 2
                        RibbonButton { showBg: false; adaptHeight: true; iconSource: RibbonIcons.ZoomOut; tipText: qsTr("Zoom Out"); enabled: contentArea.zoomLevel > contentArea.minZoom; onClicked: contentArea.zoomOut() }
                        RibbonText {
                            text: Math.round(contentArea.zoomLevel * 100) + "%"
                            font.pixelSize: 12; font.bold: true; horizontalAlignment: Text.AlignHCenter; Layout.minimumWidth: 40
                            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; hoverEnabled: true; onClicked: contentArea.resetZoom(); ToolTip.visible: containsMouse; ToolTip.text: qsTr("Reset 100%") }
                        }
                        RibbonButton { showBg: false; adaptHeight: true; iconSource: RibbonIcons.ZoomIn; tipText: qsTr("Zoom In"); enabled: contentArea.zoomLevel < contentArea.maxZoom; onClicked: contentArea.zoomIn() }
                        Rectangle { width: 1; height: 20; color: RibbonTheme.isDarkMode ? "#3a3a5a" : "#d0d4e8"; Layout.leftMargin: 2; Layout.rightMargin: 2 }
                        RibbonButton { showBg: false; adaptHeight: true; iconSource: RibbonIcons.ZoomFit; tipText: qsTr("Fit"); onClicked: contentArea.resetZoom() }
                        RibbonButton { showBg: false; adaptHeight: true; iconSource: RibbonIcons.ArrowReset; tipText: qsTr("Reset"); onClicked: contentArea.resetZoom() }
                    }
                }
            }
            RibbonButton {
                anchors { right: parent.right; bottom: parent.bottom; margins: 16 }
                visible: !zoomPanel.zoomPanelVisible && contentArea.flowchartTabOpen && contentArea.activeTab === "flowchart"
                iconSource: RibbonIcons.ZoomIn; tipText: qsTr("Show View Controls")
                onClicked: zoomPanel.zoomPanelVisible = true
            }

            // ════════════════════════════════════════════════════════════
            // BILLS LIST TAB
            // ════════════════════════════════════════════════════════════
            Item {
                id: billsPanel
                anchors.fill: parent
                visible: contentArea.billsTabOpen && contentArea.activeTab === "bills"

                property var selectedBill: null

                property int billOverdueCount: {
                    var n = 0
                    for (var i = 0; i < flowChart.billNodes.length; i++)
                        if (flowChart.billNodes[i] && flowChart.billNodes[i].overdue) n++
                    return n
                }

                function categoryColor(cat) {
                    if (cat === "water")       return "#4ee0f7"
                    if (cat === "electricity") return "#f7a54e"
                    if (cat === "telephone")   return "#9c6ef7"
                    if (cat === "internet")    return "#4caf7d"
                    if (cat === "gas")         return "#f0a030"
                    if (cat === "waste")       return "#60c060"
                    return "#4e8ef7"
                }

                // ── LIST VIEW ──────────────────────────────────────────
                Item {
                    anchors.fill: parent
                    visible: billsPanel.selectedBill === null

                    ColumnLayout {
                        anchors.fill: parent; spacing: 0

                        Rectangle {
                            Layout.fillWidth: true; height: 54; z: 1
                            color: RibbonTheme.isDarkMode ? "#1c1c2e" : "#ffffff"
                            layer.enabled: true; layer.effect: RibbonShadow {}
                            RowLayout {
                                anchors { fill: parent; leftMargin: 18; rightMargin: 12 }
                                spacing: 10
                                Rectangle {
                                    width: 34; height: 34; radius: 9; color: "#4e8ef7"
                                    RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Document; iconSize: 16; color: "white" }
                                }
                                RibbonText { text: qsTr("Bills"); font.pixelSize: 16; font.bold: true; Layout.fillWidth: true }
                                RibbonButton {
                                    showBg: false; adaptHeight: true; iconSource: RibbonIcons.ArrowClockwise; tipText: qsTr("Refresh")
                                    onClicked: appController.fetchBills()
                                }
                                RibbonButton {
                                    iconSource: RibbonIcons.Add; text: qsTr("New Bill")
                                    onClicked: { contentArea.createBillTabOpen = true; contentArea.activeTab = "createBill" }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true; height: 38
                            color: RibbonTheme.isDarkMode ? "#16162a" : "#f4f5fb"
                            RowLayout {
                                anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                spacing: 20
                                RibbonIcon { iconSource: RibbonIcons.Document; iconSize: 12; opacity: 0.55 }
                                RibbonText { text: qsTr("Total: ") + flowChart.billNodes.length; font.pixelSize: 11; opacity: 0.7 }
                                RibbonIcon {
                                    iconSource: RibbonIcons.Warning; iconSize: 12
                                    color: billsPanel.billOverdueCount > 0 ? "#ef4444" : (RibbonTheme.isDarkMode ? "white" : "black")
                                    opacity: billsPanel.billOverdueCount > 0 ? 1.0 : 0.55
                                }
                                RibbonText {
                                    text: qsTr("Overdue: ") + billsPanel.billOverdueCount
                                    font.pixelSize: 11
                                    color: billsPanel.billOverdueCount > 0 ? "#ef4444" : (RibbonTheme.isDarkMode ? "white" : "black")
                                    opacity: billsPanel.billOverdueCount > 0 ? 1.0 : 0.7
                                }
                                Item { Layout.fillWidth: true }
                            }
                        }

                        ListView {
                            id: billListView
                            Layout.fillWidth: true; Layout.fillHeight: true
                            model: flowChart.billNodes; clip: true; spacing: 0

                            Column {
                                anchors.centerIn: parent; spacing: 12
                                visible: billListView.count === 0
                                RibbonIcon { anchors.horizontalCenter: parent.horizontalCenter; iconSource: RibbonIcons.Document; iconSize: 38; opacity: 0.15 }
                                RibbonText { anchors.horizontalCenter: parent.horizontalCenter; text: qsTr("No bills yet"); font.pixelSize: 14; opacity: 0.35 }
                                RibbonButton {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: qsTr("Create first bill"); iconSource: RibbonIcons.Add
                                    onClicked: { contentArea.createBillTabOpen = true; contentArea.activeTab = "createBill" }
                                }
                            }

                            delegate: Rectangle {
                                required property var modelData
                                required property int index
                                width: billListView.width; height: 64
                                color: bRowMA.containsMouse
                                       ? (RibbonTheme.isDarkMode ? "#ffffff08" : "#f0f4ff")
                                       : (RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff")
                                Rectangle {
                                    anchors.bottom: parent.bottom; width: parent.width; height: 1
                                    color: RibbonTheme.isDarkMode ? "#2a2a42" : "#eaecf4"
                                }
                                MouseArea {
                                    id: bRowMA
                                    anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                                    onClicked: billsPanel.selectedBill = modelData
                                }
                                RowLayout {
                                    anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                    spacing: 14
                                    Rectangle {
                                        width: 4; height: 38; radius: 2
                                        color: billsPanel.categoryColor(modelData.category)
                                    }
                                    Column {
                                        Layout.fillWidth: true; spacing: 3
                                        RibbonText { text: modelData.name; font.pixelSize: 13; font.bold: true }
                                        RibbonText {
                                            visible: modelData.description !== "" && modelData.description !== undefined
                                            text: modelData.description || ""
                                            font.pixelSize: 11; opacity: 0.5
                                        }
                                    }
                                    Rectangle {
                                        height: 20; radius: 10; width: 90
                                        color: modelData.payment_mode === "recurring" ? "#4e8ef718" : "#9c6ef718"
                                        RibbonText {
                                            anchors.centerIn: parent
                                            text: modelData.payment_mode === "recurring" ? qsTr("Recurring") : qsTr("One-time")
                                            font.pixelSize: 10
                                            color: modelData.payment_mode === "recurring" ? "#4e8ef7" : "#9c6ef7"
                                        }
                                    }
                                    Column {
                                        spacing: 2; visible: modelData.due_date !== "" && modelData.due_date !== undefined
                                        RibbonText { text: qsTr("Due"); font.pixelSize: 10; opacity: 0.45 }
                                        RibbonText {
                                            text: modelData.due_date || ""
                                            font.pixelSize: 11; font.bold: true
                                            color: modelData.overdue ? "#ef4444" : (RibbonTheme.isDarkMode ? "white" : "#222")
                                        }
                                    }
                                    Column {
                                        spacing: 2
                                        RibbonText { text: qsTr("Amount"); font.pixelSize: 10; opacity: 0.45 }
                                        RibbonText {
                                            text: "$ " + parseFloat(modelData.amount || 0).toFixed(2)
                                            font.pixelSize: 13; font.bold: true; color: "#4e8ef7"
                                        }
                                    }
                                    Rectangle {
                                        visible: modelData.overdue === true
                                        width: 68; height: 22; radius: 11; color: "#ef444420"
                                        RibbonText {
                                            anchors.centerIn: parent
                                            text: qsTr("Overdue"); font.pixelSize: 10; color: "#ef4444"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // ── DETAIL VIEW ────────────────────────────────────────
                Item {
                    id: billDetailPane
                    anchors.fill: parent
                    visible: billsPanel.selectedBill !== null

                    property var  bill: billsPanel.selectedBill

                    // Days until due_date (negative = overdue)
                    property int daysUntilDue: {
                        var b = billDetailPane.bill
                        if (!b || !b.due_date || b.due_date === "") return 0
                        var parts = b.due_date.split("-")
                        if (parts.length !== 3) return 0
                        var today = new Date()
                        today.setHours(0, 0, 0, 0)
                        var due = new Date(parseInt(parts[0]), parseInt(parts[1]) - 1, parseInt(parts[2]))
                        return Math.round((due.getTime() - today.getTime()) / 86400000)
                    }

                    // Elapsed % of billing cycle
                    property real progressPct: {
                        var b = billDetailPane.bill
                        if (!b || !b.due_date || b.due_date === "") return 0.0
                        var d = billDetailPane.daysUntilDue
                        if (d <= 0) return 100.0
                        var cycle = 30
                        var rt = b.recurring_type || ""
                        if      (rt === "per_day")     cycle = 1
                        else if (rt === "per_week")    cycle = 7
                        else if (rt === "per_month")   cycle = 30
                        else if (rt === "per_quarter") cycle = 91
                        else if (rt === "per_year")    cycle = 365
                        var elapsed = cycle - d
                        if (elapsed < 0) elapsed = 0
                        return Math.min(100.0, elapsed / cycle * 100.0)
                    }

                    ColumnLayout {
                        anchors.fill: parent; spacing: 0

                        // ── Header bar ──────────────────────────────────
                        Rectangle {
                            Layout.fillWidth: true; height: 54; z: 1
                            color: RibbonTheme.isDarkMode ? "#1c1c2e" : "#ffffff"
                            layer.enabled: true; layer.effect: RibbonShadow {}
                            RowLayout {
                                anchors { fill: parent; leftMargin: 10; rightMargin: 12 }
                                spacing: 8
                                RibbonButton {
                                    showBg: false; adaptHeight: true; iconSource: RibbonIcons.ArrowLeft; tipText: qsTr("Back to Bills")
                                    onClicked: billsPanel.selectedBill = null
                                }
                                RibbonText {
                                    text: billDetailPane.bill ? (billDetailPane.bill.name || "") : ""
                                    font.pixelSize: 14; font.bold: true; Layout.fillWidth: true
                                }
                                Rectangle {
                                    visible: billDetailPane.bill && billDetailPane.bill.overdue === true
                                    width: 68; height: 22; radius: 11; color: "#ef444422"
                                    RibbonText { anchors.centerIn: parent; text: qsTr("Overdue"); font.pixelSize: 10; color: "#ef4444" }
                                }
                                RibbonButton {
                                    iconSource: RibbonIcons.Add; adaptHeight: true; text: qsTr("New Bill")
                                    onClicked: { contentArea.createBillTabOpen = true; contentArea.activeTab = "createBill" }
                                }
                            }
                        }

                        // ── Scrollable body ──────────────────────────────
                        ScrollView {
                            id: billDetailScroll
                            Layout.fillWidth: true; Layout.fillHeight: true; clip: true

                            Column {
                                width: billDetailScroll.width; spacing: 0

                                // ── Hero card ────────────────────────────
                                Rectangle {
                                    width: parent.width
                                    height: bdHeroCol.implicitHeight + 26
                                    color: (billDetailPane.bill && billDetailPane.bill.overdue)
                                           ? (RibbonTheme.isDarkMode ? "#2a1515" : "#fff5f5")
                                           : (RibbonTheme.isDarkMode ? "#1e1e30" : "#fffef9")

                                    // Overdue animated left stripe
                                    Rectangle {
                                        id: bdOverdueStripe
                                        visible: billDetailPane.bill && billDetailPane.bill.overdue === true
                                        x: 0; y: 0; width: 5; height: parent.height
                                        gradient: Gradient {
                                            GradientStop { id: bdSG1; position: 0.0; color: "#ef4444" }
                                            GradientStop { id: bdSG2; position: 0.38; color: "#991b1b" }
                                            GradientStop { id: bdSG3; position: 1.0; color: "#f87171" }
                                        }
                                        SequentialAnimation {
                                            running: bdOverdueStripe.visible; loops: Animation.Infinite
                                            NumberAnimation { target: bdSG2; property: "position"; from: 0.38; to: 0.62; duration: 920; easing.type: Easing.InOutSine }
                                            NumberAnimation { target: bdSG2; property: "position"; from: 0.62; to: 0.38; duration: 920; easing.type: Easing.InOutSine }
                                        }
                                    }

                                    // Bottom border
                                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a42" : "#eaecf4" }

                                    Column {
                                        id: bdHeroCol
                                        anchors { left: parent.left; right: parent.right
                                                  leftMargin: billDetailPane.bill && billDetailPane.bill.overdue ? 20 : 16
                                                  rightMargin: 12; top: parent.top; topMargin: 13 }
                                        spacing: 9

                                        // Bill name
                                        Text {
                                            width: parent.width
                                            text: billDetailPane.bill ? (billDetailPane.bill.name || "") : ""
                                            font.pixelSize: 20; font.bold: true
                                            color: RibbonTheme.isDarkMode ? "#ffffff" : "#1a1a2e"
                                        }

                                        // Pill flow
                                        Flow {
                                            id: bdPillFlow
                                            width: parent.width; spacing: 5

                                            // Overdue
                                            Rectangle {
                                                visible: billDetailPane.bill && billDetailPane.bill.overdue === true
                                                height: 20; radius: 999; color: "#ef444418"
                                                border.color: "#ef444460"; border.width: 1
                                                width: bdPillOv.implicitWidth + 20
                                                Text { id: bdPillOv; anchors.centerIn: parent; text: qsTr("OVERDUE"); font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: "#ef4444" }
                                            }
                                            // Category
                                            Rectangle {
                                                visible: billDetailPane.bill && billDetailPane.bill.category_label !== "" && billDetailPane.bill.category_label !== undefined
                                                height: 20; radius: 999; color: RibbonTheme.isDarkMode ? "#ffffff14" : "#1a1a2e12"
                                                border.color: RibbonTheme.isDarkMode ? "#ffffff28" : "#1a1a2e28"; border.width: 1
                                                width: bdPillCat.implicitWidth + 20
                                                Text { id: bdPillCat; anchors.centerIn: parent; text: billDetailPane.bill ? (billDetailPane.bill.category_label || "").toUpperCase() : ""; font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: RibbonTheme.isDarkMode ? "#ccccdd" : "#555566" }
                                            }
                                            // Payment mode
                                            Rectangle {
                                                height: 20; radius: 999; color: RibbonTheme.isDarkMode ? "#ffffff14" : "#1a1a2e12"
                                                border.color: RibbonTheme.isDarkMode ? "#ffffff28" : "#1a1a2e28"; border.width: 1
                                                width: bdPillMode.implicitWidth + 20
                                                Text { id: bdPillMode; anchors.centerIn: parent; text: (billDetailPane.bill && billDetailPane.bill.payment_mode === "recurring") ? qsTr("RECURRING") : qsTr("ONE-TIME"); font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: RibbonTheme.isDarkMode ? "#ccccdd" : "#555566" }
                                            }
                                            // Recurring cadence
                                            Rectangle {
                                                visible: billDetailPane.bill && billDetailPane.bill.recurring_type !== ""
                                                height: 20; radius: 999; color: RibbonTheme.isDarkMode ? "#ffffff14" : "#1a1a2e12"
                                                border.color: RibbonTheme.isDarkMode ? "#ffffff28" : "#1a1a2e28"; border.width: 1
                                                width: bdPillRec.implicitWidth + 20
                                                Text { id: bdPillRec; anchors.centerIn: parent; text: billDetailPane.bill ? (billDetailPane.bill.recurring_type || "").replace("per_", "PER ").toUpperCase() : ""; font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: RibbonTheme.isDarkMode ? "#ccccdd" : "#555566" }
                                            }
                                            // Through year
                                            Rectangle {
                                                visible: billDetailPane.bill && billDetailPane.bill.agreement_until !== "" && billDetailPane.bill.agreement_until !== "0"
                                                height: 20; radius: 999; color: RibbonTheme.isDarkMode ? "#ffffff14" : "#1a1a2e12"
                                                border.color: RibbonTheme.isDarkMode ? "#ffffff28" : "#1a1a2e28"; border.width: 1
                                                width: bdPillThru.implicitWidth + 20
                                                Text { id: bdPillThru; anchors.centerIn: parent; text: billDetailPane.bill ? qsTr("THROUGH ") + (billDetailPane.bill.agreement_until || "") : ""; font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: RibbonTheme.isDarkMode ? "#ccccdd" : "#555566" }
                                            }
                                            // Employee
                                            Rectangle {
                                                visible: billDetailPane.bill && billDetailPane.bill.employee_name !== ""
                                                height: 20; radius: 999; color: RibbonTheme.isDarkMode ? "#ffffff14" : "#1a1a2e12"
                                                border.color: RibbonTheme.isDarkMode ? "#ffffff28" : "#1a1a2e28"; border.width: 1
                                                width: bdPillEmp.implicitWidth + 20
                                                Text { id: bdPillEmp; anchors.centerIn: parent; text: billDetailPane.bill ? (billDetailPane.bill.employee_name || "").toUpperCase() : ""; font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: RibbonTheme.isDarkMode ? "#ccccdd" : "#555566" }
                                            }
                                            // Due date
                                            Rectangle {
                                                visible: billDetailPane.bill && billDetailPane.bill.actual_due_date_fmt !== "" && billDetailPane.bill.actual_due_date_fmt !== undefined
                                                height: 20; radius: 999
                                                color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#ef444418" : (RibbonTheme.isDarkMode ? "#ffffff14" : "#1a1a2e12")
                                                border.color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#ef444460" : (RibbonTheme.isDarkMode ? "#ffffff28" : "#1a1a2e28"); border.width: 1
                                                width: bdPillDue.implicitWidth + 20
                                                Text { id: bdPillDue; anchors.centerIn: parent; text: billDetailPane.bill ? qsTr("DUE ") + (billDetailPane.bill.actual_due_date_fmt || "").toUpperCase() : ""; font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#ef4444" : (RibbonTheme.isDarkMode ? "#ccccdd" : "#555566") }
                                            }
                                            // First installment
                                            Rectangle {
                                                visible: billDetailPane.bill && billDetailPane.bill.first_install_date_fmt !== "" && billDetailPane.bill.first_install_date_fmt !== undefined
                                                height: 20; radius: 999; color: RibbonTheme.isDarkMode ? "#ffffff14" : "#1a1a2e12"
                                                border.color: RibbonTheme.isDarkMode ? "#ffffff28" : "#1a1a2e28"; border.width: 1
                                                width: bdPillInst.implicitWidth + 20
                                                Text { id: bdPillInst; anchors.centerIn: parent; text: billDetailPane.bill ? qsTr("FIRST INSTALLMENT ") + (billDetailPane.bill.first_install_date_fmt || "").toUpperCase() : ""; font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.5; color: RibbonTheme.isDarkMode ? "#ccccdd" : "#555566" }
                                            }
                                        }
                                    }
                                }

                                // ════════════════════════════════════════
                                // BILL DETAILS
                                // ════════════════════════════════════════
                                Column {
                                    width: parent.width; spacing: 0

                                    // Overdue alert
                                    Rectangle {
                                        width: parent.width
                                        height: bdAlertRow.implicitHeight + 20
                                        visible: billDetailPane.bill && billDetailPane.bill.overdue === true
                                        color: RibbonTheme.isDarkMode ? "#2a1a1a" : "#fff5f5"
                                        border.color: RibbonTheme.isDarkMode ? "#f8717155" : "#fca5a5"; border.width: 1
                                        RowLayout {
                                            id: bdAlertRow
                                            anchors { left: parent.left; right: parent.right; top: parent.top; margins: 10 }
                                            spacing: 10
                                            RibbonIcon { iconSource: RibbonIcons.Warning; iconSize: 14; color: "#f87171"; Layout.alignment: Qt.AlignTop }
                                            Column {
                                                Layout.fillWidth: true; spacing: 3
                                                Text { text: qsTr("Unpaid billing due"); font.pixelSize: 12; font.bold: true; color: RibbonTheme.isDarkMode ? "#fca5a5" : "#dc2626" }
                                                Text {
                                                    width: parent.width; wrapMode: Text.WordWrap
                                                    text: qsTr("At least one scheduled bill date on or before today has no ledger payment recorded. Log the payment so your schedule stays accurate.")
                                                    font.pixelSize: 11; lineHeight: 1.42
                                                    color: RibbonTheme.isDarkMode ? "#fecaca" : "#7f1d1d"
                                                }
                                            }
                                        }
                                    }

                                    // Spacer before countdown card
                                    Item { width: parent.width; height: 12 }

                                    // Next payment countdown card
                                    Item {
                                        width: parent.width; height: bdCountdownCard.height + 12
                                        visible: billDetailPane.bill && billDetailPane.bill.due_date !== ""

                                        Rectangle {
                                            id: bdCountdownCard
                                            anchors { left: parent.left; right: parent.right; leftMargin: 12; rightMargin: 12 }
                                            height: bdCountdownInner.implicitHeight + 28; radius: 10
                                            color: (billDetailPane.bill && billDetailPane.bill.overdue)
                                                   ? (RibbonTheme.isDarkMode ? "#2a1010" : "#fff5f5")
                                                   : (RibbonTheme.isDarkMode ? "#1e1e32" : "#ffffff")
                                            border.color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#ef444455" : (RibbonTheme.isDarkMode ? "#2a2a42" : "#e0e4f0"); border.width: 1
                                            layer.enabled: true; layer.effect: RibbonShadow {}

                                            Column {
                                                id: bdCountdownInner
                                                anchors { left: parent.left; right: parent.right; top: parent.top; margins: 14 }
                                                spacing: 12

                                                RowLayout {
                                                    width: parent.width

                                                    Column {
                                                        spacing: 0; Layout.fillWidth: true
                                                        Text {
                                                            text: qsTr("NEXT BILL PAYMENT")
                                                            font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.8
                                                            color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#f87171" : (RibbonTheme.isDarkMode ? "#9999bb" : "#888899")
                                                        }
                                                        Text {
                                                            text: {
                                                                var d = billDetailPane.daysUntilDue
                                                                if (d === 0) return qsTr("Today")
                                                                return Math.abs(d).toString()
                                                            }
                                                            font.pixelSize: 40; font.bold: true; lineHeight: 1.0
                                                            color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#f87171" : (RibbonTheme.isDarkMode ? "#4e8ef7" : "#1a1a2e")
                                                        }
                                                        Text {
                                                            visible: billDetailPane.daysUntilDue !== 0
                                                            text: {
                                                                var d = billDetailPane.daysUntilDue
                                                                if (d < 0) return qsTr("days overdue")
                                                                return d === 1 ? qsTr("day left") : qsTr("days left")
                                                            }
                                                            font.pixelSize: 11
                                                            color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#fca5a5" : (RibbonTheme.isDarkMode ? "#7777aa" : "#888899")
                                                        }
                                                    }

                                                    Column {
                                                        spacing: 3
                                                        Text {
                                                            text: billDetailPane.bill ? (billDetailPane.bill.due_date_fmt || billDetailPane.bill.due_date || "") : ""
                                                            font.pixelSize: 14; font.bold: true
                                                            color: RibbonTheme.isDarkMode ? "white" : "#1a1a2e"
                                                            horizontalAlignment: Text.AlignRight
                                                        }
                                                        Text {
                                                            visible: billDetailPane.bill && billDetailPane.bill.recurring_type !== ""
                                                            text: billDetailPane.bill ? (billDetailPane.bill.recurring_type || "").replace("per_", "Per ") : ""
                                                            font.pixelSize: 11
                                                            color: RibbonTheme.isDarkMode ? "#9999bb" : "#888899"
                                                            horizontalAlignment: Text.AlignRight
                                                        }
                                                    }
                                                }

                                                // Progress bar
                                                Column {
                                                    width: parent.width; spacing: 5
                                                    RowLayout {
                                                        width: parent.width
                                                        Text { text: qsTr("APPROACHING DUE DATE"); font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.6; color: RibbonTheme.isDarkMode ? "#9999bb" : "#888899"; Layout.fillWidth: true }
                                                        Text { text: Math.round(billDetailPane.progressPct) + "%"; font.pixelSize: 9; font.bold: true; color: RibbonTheme.isDarkMode ? "#9999bb" : "#888899" }
                                                    }
                                                    Rectangle {
                                                        width: parent.width; height: 7; radius: 999
                                                        color: RibbonTheme.isDarkMode ? "#2a2a42" : "#e8eaf0"
                                                        Rectangle {
                                                            width: parent.width * (billDetailPane.progressPct / 100.0)
                                                            height: parent.height; radius: 999
                                                            color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#ef4444"
                                                                   : (billDetailPane.progressPct >= 85 ? "#f59e0b" : "#4e8ef7")
                                                            Behavior on width { NumberAnimation { duration: 450; easing.type: Easing.OutCubic } }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // Cost + Reminder cards
                                    Item {
                                        width: parent.width
                                        height: bdCostCard.height + 12

                                        // Cost card (adjusts width when reminder present)
                                        Rectangle {
                                            id: bdCostCard
                                            x: 12; y: 0
                                            width: (billDetailPane.bill && billDetailPane.bill.remind_days > 0)
                                                   ? (parent.width - 34) * 0.62
                                                   : (parent.width - 24)
                                            height: 88; radius: 10
                                            color: RibbonTheme.isDarkMode ? "#1e1e32" : "#ffffff"
                                            border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#e0e4f0"; border.width: 1
                                            layer.enabled: true; layer.effect: RibbonShadow {}
                                            Behavior on width { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }

                                            // Left accent bar
                                            Rectangle { x: 0; y: 0; width: 4; height: parent.height; radius: 3; color: "#4e8ef7" }

                                            Column {
                                                anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; leftMargin: 16; rightMargin: 12 }
                                                spacing: 3
                                                Text {
                                                    text: (billDetailPane.bill && billDetailPane.bill.payment_mode !== "recurring") ? qsTr("PAYMENT AMOUNT") : qsTr("RECURRING COST")
                                                    font.pixelSize: 8; font.bold: true; font.letterSpacing: 0.7
                                                    color: RibbonTheme.isDarkMode ? "#9999bb" : "#888899"
                                                }
                                                Text {
                                                    text: billDetailPane.bill ? parseFloat(billDetailPane.bill.amount || 0).toFixed(2) : "0.00"
                                                    font.pixelSize: 26; font.bold: true
                                                    color: RibbonTheme.isDarkMode ? "#4e8ef7" : "#1a1a2e"
                                                }
                                                Text {
                                                    visible: billDetailPane.bill && billDetailPane.bill.recurring_type !== ""
                                                    text: qsTr("PER BILLING PERIOD · ") + (billDetailPane.bill ? (billDetailPane.bill.recurring_type || "").replace("per_", "PER ").toUpperCase() : "")
                                                    font.pixelSize: 8; font.bold: true; font.letterSpacing: 0.4
                                                    color: RibbonTheme.isDarkMode ? "#9999bb" : "#888899"
                                                }
                                            }
                                        }

                                        // Reminder card
                                        Rectangle {
                                            visible: billDetailPane.bill && billDetailPane.bill.remind_days > 0
                                            x: bdCostCard.x + bdCostCard.width + 10
                                            y: 0
                                            width: parent.width - x - 12
                                            height: bdCostCard.height; radius: 10
                                            color: RibbonTheme.isDarkMode ? "#1e1e32" : "#fffbeb"
                                            border.color: RibbonTheme.isDarkMode ? "#3a3020" : "#fbbf2455"; border.width: 1
                                            layer.enabled: true; layer.effect: RibbonShadow {}

                                            Column {
                                                anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; leftMargin: 12; rightMargin: 8 }
                                                spacing: 4
                                                Rectangle {
                                                    width: 30; height: 30; radius: 8
                                                    color: RibbonTheme.isDarkMode ? "#3a2a00" : "#fef3c7"
                                                    border.color: "#fbbf2440"; border.width: 1
                                                    RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Bell; iconSize: 13; color: "#f59e0b" }
                                                }
                                                Text {
                                                    text: billDetailPane.bill ? (billDetailPane.bill.remind_days + " " + qsTr("days ahead")) : ""
                                                    font.pixelSize: 12; font.bold: true
                                                    color: RibbonTheme.isDarkMode ? "#fcd34d" : "#92400e"
                                                }
                                                Text {
                                                    text: qsTr("REMINDER BEFORE\nPERIOD END")
                                                    font.pixelSize: 8; font.bold: true; font.letterSpacing: 0.5
                                                    color: RibbonTheme.isDarkMode ? "#9999bb" : "#888899"
                                                }
                                            }
                                        }
                                    }

                                    // ── Dates & billing ──────────────────────
                                    Rectangle {
                                        width: parent.width; height: 32; color: RibbonTheme.isDarkMode ? "#16162a" : "#f4f5fb"
                                        RowLayout {
                                            anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            spacing: 8
                                            RibbonIcon { iconSource: RibbonIcons.Calendar; iconSize: 12; opacity: 0.55 }
                                            RibbonText { text: qsTr("Dates & Billing"); font.pixelSize: 11; font.bold: true; opacity: 0.75; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle {
                                        width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.actual_due_date_fmt !== ""
                                        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Due date"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.actual_due_date_fmt || "") : ""; font.pixelSize: 12; font.bold: true; color: (billDetailPane.bill && billDetailPane.bill.overdue) ? "#ef4444" : (RibbonTheme.isDarkMode ? "white" : "#1a1a2e"); Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle {
                                        width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.first_install_date_fmt !== ""
                                        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("First installment"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.first_install_date_fmt || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle {
                                        width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.recurring_type !== ""
                                        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Cadence"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.recurring_type || "").replace("per_", "Per ") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle {
                                        width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.agreement_until !== "" && billDetailPane.bill.agreement_until !== "0"
                                        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Valid until"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.agreement_until || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle {
                                        width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.remind_days > 0
                                        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Reminder"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.remind_days + qsTr(" days before end")) : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }

                                    // ── Assigned to ───────────────────────
                                    Rectangle {
                                        width: parent.width; height: 32; color: RibbonTheme.isDarkMode ? "#16162a" : "#f4f5fb"
                                        visible: billDetailPane.bill && (billDetailPane.bill.property_name !== "" || billDetailPane.bill.employee_name !== "" || billDetailPane.bill.modification_name !== "" || billDetailPane.bill.department_name !== "" || billDetailPane.bill.rental_type !== "" || billDetailPane.bill.branch_name !== "")
                                        RowLayout {
                                            anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            spacing: 8
                                            RibbonIcon { iconSource: RibbonIcons.Person; iconSize: 12; opacity: 0.55 }
                                            RibbonText { text: qsTr("Assigned To"); font.pixelSize: 11; font.bold: true; opacity: 0.75; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle { width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.property_name !== ""; color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Property"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.property_name || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle { width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.employee_name !== ""; color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Employee"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.employee_name || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle { width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.modification_name !== ""; color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Modification"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.modification_name || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle { width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.department_name !== ""; color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Department"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.department_name || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle { width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.rental_type !== ""; color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Rental"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.rental_type || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }
                                    Rectangle { width: parent.width; height: 44; visible: billDetailPane.bill && billDetailPane.bill.branch_name !== ""; color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        RowLayout { anchors { fill: parent; leftMargin: 18; rightMargin: 18 }
                                            RibbonText { text: qsTr("Branch"); font.pixelSize: 12; opacity: 0.55; Layout.preferredWidth: 140 }
                                            RibbonText { text: billDetailPane.bill ? (billDetailPane.bill.branch_name || "") : ""; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                                        }
                                    }

                                    // ── Description ───────────────────────
                                    Rectangle {
                                        width: parent.width
                                        height: bdDescInner.implicitHeight + 28
                                        visible: billDetailPane.bill && billDetailPane.bill.description !== "" && billDetailPane.bill.description !== undefined
                                        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: RibbonTheme.isDarkMode ? "#2a2a3a" : "#f0f0f8" }
                                        Column {
                                            id: bdDescInner
                                            anchors { left: parent.left; right: parent.right; top: parent.top; leftMargin: 18; rightMargin: 18; topMargin: 12 }
                                            spacing: 6
                                            RibbonText { text: qsTr("Description"); font.pixelSize: 9; opacity: 0.45 }
                                            Text {
                                                width: parent.width; wrapMode: Text.WordWrap; lineHeight: 1.45
                                                text: billDetailPane.bill ? (billDetailPane.bill.description || "") : ""
                                                font.pixelSize: 12
                                                color: RibbonTheme.isDarkMode ? "#ccccee" : "#333355"
                                            }
                                        }
                                    }

                                    // ── Notes ─────────────────────────────
                                    Rectangle {
                                        width: parent.width
                                        height: bdNotesInner.implicitHeight + 28
                                        visible: billDetailPane.bill && billDetailPane.bill.notes !== "" && billDetailPane.bill.notes !== undefined
                                        color: RibbonTheme.isDarkMode ? "#1e1e2e" : "#ffffff"
                                        Column {
                                            id: bdNotesInner
                                            anchors { left: parent.left; right: parent.right; top: parent.top; leftMargin: 18; rightMargin: 18; topMargin: 12 }
                                            spacing: 6
                                            RibbonText { text: qsTr("Notes"); font.pixelSize: 9; opacity: 0.45 }
                                            Rectangle {
                                                width: parent.width; radius: 8
                                                height: bdNotesText.implicitHeight + 16
                                                color: RibbonTheme.isDarkMode ? "#ffffff08" : "#f4f5fb"
                                                border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#e8eaf0"; border.width: 1
                                                Text {
                                                    id: bdNotesText
                                                    x: 12; y: 8; width: parent.width - 24
                                                    text: billDetailPane.bill ? (billDetailPane.bill.notes || "") : ""
                                                    color: RibbonTheme.isDarkMode ? "#ccccee" : "#333355"
                                                    font.pixelSize: 12; wrapMode: Text.WordWrap; lineHeight: 1.45
                                                }
                                            }
                                        }
                                    }

                                    Item { width: parent.width; height: 24 }
                                }
                            }
                        }
                    }
                }
            }

            // ════════════════════════════════════════════════════════════
            // CREATE BILL TAB
            // ════════════════════════════════════════════════════════════
            Item {
                id: createBillPanel
                anchors.fill: parent
                visible: contentArea.createBillTabOpen && contentArea.activeTab === "createBill"

                property string payMode:             "recurring"
                property bool   submitting:          false
                property int    statusType:          0
                property string statusMsg:           ""
                property bool   amountVaries:        false
                property bool   splitPayment:        true
                property var    accountsList:        []
                property int    selectedAccountIdx:  0
                property string assignmentType:           "none"
                property int    assignmentTargetIdx:      0
                property bool   assignmentTargetsFetched: false
                property bool   assignmentTargetsLoading: false
                // Each resource list stored flat — avoids nested var-object access issues
                property var    abBranches:      []
                property var    abDepartments:   []
                property var    abProperties:    []
                property var    abEmployees:     []
                property var    abModifications: []
                property var    abRentals:       []
                property var    targetList:      [qsTr("— Select —")]
                property bool   targetListEmpty: true

                function rebuildTargetList(type) {
                    var src = []
                    if      (type === "branch")       src = createBillPanel.abBranches
                    else if (type === "department")   src = createBillPanel.abDepartments
                    else if (type === "property")     src = createBillPanel.abProperties
                    else if (type === "employee")     src = createBillPanel.abEmployees
                    else if (type === "modification") src = createBillPanel.abModifications
                    else if (type === "rental")       src = createBillPanel.abRentals
                    console.log("[Bill] rebuildTargetList type=" + type + " src.length=" + src.length)
                    var list = [qsTr("— Select —")]
                    for (var i = 0; i < src.length; i++) list.push(src[i].name || "")
                    createBillPanel.targetList      = list
                    createBillPanel.targetListEmpty = (src.length === 0)
                    assignTargetCombo.currentIndex  = 0
                }

                function triggerFetchTargets() {
                    if (!assignmentTargetsFetched && !assignmentTargetsLoading) {
                        console.log("[Bill] triggering fetchBillAssignmentTargets")
                        createBillPanel.assignmentTargetsLoading = true
                        appController.fetchBillAssignmentTargets()
                    }
                }

                Component.onCompleted: Qt.callLater(triggerFetchTargets)

                onVisibleChanged: {
                    if (visible) {
                        if (accountsList.length === 0)
                            Qt.callLater(function() { appController.fetchAccounts() })
                        Qt.callLater(triggerFetchTargets)
                    }
                }

                Connections {
                    target: appController
                    function onBillCreated(billName) {
                        createBillPanel.submitting = false
                        createBillPanel.statusType = 1
                        createBillPanel.statusMsg  = qsTr("Bill \"%1\" created successfully.").arg(billName)
                        // Reset all fields to defaults
                        billNameField.text             = ""
                        billCategoryCombo.currentIndex = 6
                        billSpecifyField.text          = ""
                        billDescField.text             = ""
                        createBillPanel.payMode        = "recurring"
                        billScheduleYearField.text     = (new Date().getFullYear() + 1).toString()
                        createBillPanel.amountVaries   = false
                        createBillPanel.splitPayment   = true
                        billAmountField.text           = ""
                        billCadenceCombo.currentIndex  = 1
                        billRemindField.text           = ""
                        billDueDateField.text          = ""
                        billFirstInstallField.text     = ""
                        billAccountCombo.currentIndex  = 0
                        billNotesArea.text             = ""
                        assignTypeCombo.currentIndex        = 0
                        createBillPanel.assignmentType      = "none"
                        createBillPanel.assignmentTargetIdx = 0
                        createBillPanel.rebuildTargetList("none")
                    }
                    function onBillError(message) {
                        createBillPanel.submitting = false
                        createBillPanel.statusType = 2
                        createBillPanel.statusMsg  = message
                    }
                    function onAccountsLoaded(accounts) {
                        createBillPanel.accountsList    = accounts
                        createBillPanel.selectedAccountIdx = 0
                    }
                    function onBillAssignmentTargetsLoaded(targets) {
                        console.log("[Bill] onBillAssignmentTargetsLoaded branches=" +
                            (targets.branches ? targets.branches.length : "null") +
                            " depts=" + (targets.departments ? targets.departments.length : "null") +
                            " emps=" + (targets.employees ? targets.employees.length : "null"))
                        createBillPanel.abBranches      = targets.branches       || []
                        createBillPanel.abDepartments   = targets.departments    || []
                        createBillPanel.abProperties    = targets.properties     || []
                        createBillPanel.abEmployees     = targets.employees      || []
                        createBillPanel.abModifications = targets.modifications  || []
                        createBillPanel.abRentals       = targets.rentals        || []
                        createBillPanel.assignmentTargetsFetched = true
                        createBillPanel.assignmentTargetsLoading = false
                        createBillPanel.rebuildTargetList(createBillPanel.assignmentType)
                    }
                    function onBillAssignmentTargetsError(message) {
                        console.log("[Bill] onBillAssignmentTargetsError: " + message)
                        createBillPanel.assignmentTargetsLoading = false
                    }
                }

                // ── Header bar ───────────────────────────────────────────
                Rectangle {
                    id: billHeader
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    height: 48
                    color: RibbonTheme.isDarkMode ? "#13131f" : "#e2e4ef"
                    Rectangle {
                        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                        height: 1
                        color: RibbonTheme.isDarkMode ? "#2a2a42" : "#cdd0e0"
                    }
                    RowLayout {
                        anchors { fill: parent; leftMargin: 16; rightMargin: 8 }
                        spacing: 10
                        RibbonIcon { iconSource: RibbonIcons.Document; iconSize: 16; color: "#4e8ef7" }
                        RibbonText { text: qsTr("Create Bill"); font.pixelSize: 14; font.bold: true; Layout.fillWidth: true }
                        RibbonText {
                            visible: createBillPanel.statusType !== 0
                            text: createBillPanel.statusMsg
                            color: createBillPanel.statusType === 1 ? "#4caf7d" : "#f74e6c"
                            font.pixelSize: 12
                        }
                    }
                }

                // ── Scrollable form ───────────────────────────────────────
                Flickable {
                    id: billFormFlick
                    anchors { top: billHeader.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
                    contentWidth: width
                    contentHeight: billFormOuter.implicitHeight + 40
                    clip: true

                    ColumnLayout {
                        id: billFormOuter
                        anchors { top: parent.top; left: parent.left; right: parent.right; margins: 20; topMargin: 20 }
                        spacing: 16

                        // ── Intro ─────────────────────────────────────────
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 14
                            Rectangle {
                                width: 46; height: 46; radius: 10; color: "#d97706"
                                RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Document; iconSize: 22; color: "white" }
                            }
                            ColumnLayout {
                                spacing: 3; Layout.fillWidth: true
                                RibbonText { text: qsTr("Add your first bill"); font.pixelSize: 16; font.bold: true }
                                RibbonText {
                                    Layout.fillWidth: true
                                    text: qsTr("Utilities and services with a fixed cycle and amount — same payment schedule and ledger flow as loans and rentals.")
                                    font.pixelSize: 11; opacity: 0.55; wrapMode: Text.WordWrap
                                }
                            }
                        }

                        // ── Bill card ─────────────────────────────────────
                        Rectangle {
                            Layout.fillWidth: true
                            implicitHeight: billCardBody.implicitHeight + 40
                            radius: 10
                            color: RibbonTheme.isDarkMode ? "#1c1c2e" : "#ffffff"
                            border.width: 1
                            border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#e5e7eb"
                            layer.enabled: true
                            layer.effect: RibbonShadow {}

                            ColumnLayout {
                                id: billCardBody
                                x: 20; y: 16
                                width: parent.width - 40
                                spacing: 16

                                RowLayout { spacing: 10
                                    Rectangle { width: 28; height: 28; radius: 6; color: "#d97706"
                                        RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Document; iconSize: 14; color: "white" }
                                    }
                                    RibbonText { text: qsTr("Bill"); font.pixelSize: 14; font.bold: true }
                                }

                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("NAME"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    TextField {
                                        id: billNameField
                                        Layout.fillWidth: true
                                        placeholderText: qsTr("Short label on your dashboard")
                                        font.pixelSize: 13; leftPadding: 10; rightPadding: 10; topPadding: 9; bottomPadding: 9
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billNameField.activeFocus ? 2 : 1
                                            border.color: billNameField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }

                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("BILL TYPE"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    ComboBox {
                                        id: billCategoryCombo
                                        Layout.fillWidth: true; font.pixelSize: 13; currentIndex: 6
                                        model: [qsTr("Water"), qsTr("Electricity"), qsTr("Telephone"), qsTr("Internet"), qsTr("Gas"), qsTr("Waste"), qsTr("Other (specify)")]
                                        onCurrentIndexChanged: if (currentIndex === 0) createBillPanel.amountVaries = true
                                        contentItem: Text {
                                            leftPadding: 10; text: billCategoryCombo.displayText; font: billCategoryCombo.font
                                            color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"; verticalAlignment: Text.AlignVCenter
                                        }
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billCategoryCombo.down ? 2 : 1
                                            border.color: billCategoryCombo.down ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                    }
                                }

                                ColumnLayout {
                                    spacing: 5; Layout.fillWidth: true
                                    visible: billCategoryCombo.currentIndex === 6
                                    RibbonText { text: qsTr("SPECIFY TYPE when Other"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    TextField {
                                        id: billSpecifyField
                                        Layout.fillWidth: true
                                        placeholderText: qsTr("e.g. Security, Software subscription")
                                        font.pixelSize: 13; leftPadding: 10; rightPadding: 10; topPadding: 9; bottomPadding: 9
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billSpecifyField.activeFocus ? 2 : 1
                                            border.color: billSpecifyField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }

                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("DESCRIPTION optional"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    TextArea {
                                        id: billDescField
                                        Layout.fillWidth: true
                                        placeholderText: qsTr("Vendor, account number, or service notes")
                                        font.pixelSize: 13; wrapMode: TextArea.Wrap; height: 72
                                        leftPadding: 10; rightPadding: 10; topPadding: 8; bottomPadding: 8
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billDescField.activeFocus ? 2 : 1
                                            border.color: billDescField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }

                                ColumnLayout { spacing: 8; Layout.fillWidth: true
                                    RibbonText { text: qsTr("PAYMENT PATTERN"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    RibbonText {
                                        Layout.fillWidth: true
                                        text: qsTr("Recurring bills repeat on a schedule you control. One-time is a single payment with a due or anchor date.")
                                        font.pixelSize: 11; opacity: 0.55; wrapMode: Text.WordWrap
                                    }
                                    RowLayout { spacing: 10; Layout.fillWidth: true
                                        // Recurring card
                                        Rectangle {
                                            Layout.fillWidth: true; height: 70; radius: 8
                                            color: createBillPanel.payMode === "recurring"
                                                   ? (RibbonTheme.isDarkMode ? "#1a2a4a" : "#eff6ff")
                                                   : (RibbonTheme.isDarkMode ? "#252535" : "#f9fafb")
                                            border.width: createBillPanel.payMode === "recurring" ? 2 : 1
                                            border.color: createBillPanel.payMode === "recurring"
                                                          ? "#4e8ef7"
                                                          : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                            RowLayout {
                                                anchors { fill: parent; margins: 12 }
                                                spacing: 10
                                                Rectangle {
                                                    width: 34; height: 34; radius: 8
                                                    color: createBillPanel.payMode === "recurring" ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#e5e7eb")
                                                    RibbonIcon {
                                                        anchors.centerIn: parent; iconSource: RibbonIcons.ArrowClockwise; iconSize: 16
                                                        color: createBillPanel.payMode === "recurring" ? "white" : (RibbonTheme.isDarkMode ? "#888" : "#666")
                                                    }
                                                }
                                                ColumnLayout { spacing: 2; Layout.fillWidth: true
                                                    RibbonText { text: qsTr("Recurring"); font.pixelSize: 12; font.bold: true }
                                                    RibbonText {
                                                        Layout.fillWidth: true
                                                        text: qsTr("Cycles until the schedule end year · set cadence (month / year / day)")
                                                        font.pixelSize: 10; opacity: 0.55; wrapMode: Text.WordWrap
                                                    }
                                                }
                                                Rectangle {
                                                    width: 16; height: 16; radius: 8; color: "transparent"
                                                    border.width: 2
                                                    border.color: createBillPanel.payMode === "recurring" ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#555" : "#d1d5db")
                                                    Rectangle {
                                                        anchors.centerIn: parent; width: 8; height: 8; radius: 4; color: "#4e8ef7"
                                                        visible: createBillPanel.payMode === "recurring"
                                                    }
                                                }
                                            }
                                            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: createBillPanel.payMode = "recurring" }
                                        }

                                        // One-time card
                                        Rectangle {
                                            Layout.fillWidth: true; height: 70; radius: 8
                                            color: createBillPanel.payMode === "one_time"
                                                   ? (RibbonTheme.isDarkMode ? "#1a2a4a" : "#eff6ff")
                                                   : (RibbonTheme.isDarkMode ? "#252535" : "#f9fafb")
                                            border.width: createBillPanel.payMode === "one_time" ? 2 : 1
                                            border.color: createBillPanel.payMode === "one_time"
                                                          ? "#4e8ef7"
                                                          : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                            RowLayout {
                                                anchors { fill: parent; margins: 12 }
                                                spacing: 10
                                                Rectangle {
                                                    width: 34; height: 34; radius: 8
                                                    color: createBillPanel.payMode === "one_time" ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#e5e7eb")
                                                    RibbonIcon {
                                                        anchors.centerIn: parent; iconSource: RibbonIcons.Calendar; iconSize: 16
                                                        color: createBillPanel.payMode === "one_time" ? "white" : (RibbonTheme.isDarkMode ? "#888" : "#666")
                                                    }
                                                }
                                                ColumnLayout { spacing: 2; Layout.fillWidth: true
                                                    RibbonText { text: qsTr("One-time payment"); font.pixelSize: 12; font.bold: true }
                                                    RibbonText {
                                                        Layout.fillWidth: true
                                                        text: qsTr("One payment total · requires a due date or first installment date")
                                                        font.pixelSize: 10; opacity: 0.55; wrapMode: Text.WordWrap
                                                    }
                                                }
                                                Rectangle {
                                                    width: 16; height: 16; radius: 8; color: "transparent"
                                                    border.width: 2
                                                    border.color: createBillPanel.payMode === "one_time" ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#555" : "#d1d5db")
                                                    Rectangle {
                                                        anchors.centerIn: parent; width: 8; height: 8; radius: 4; color: "#4e8ef7"
                                                        visible: createBillPanel.payMode === "one_time"
                                                    }
                                                }
                                            }
                                            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: createBillPanel.payMode = "one_time" }
                                        }
                                    }
                                }

                                // SCHEDULE THROUGH YEAR — in Bill section (web parity), grayed when one-time
                                ColumnLayout {
                                    spacing: 5; Layout.fillWidth: true
                                    opacity: createBillPanel.payMode === "recurring" ? 1.0 : 0.5
                                    RibbonText { text: qsTr("SCHEDULE THROUGH YEAR"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    TextField {
                                        id: billScheduleYearField
                                        Layout.fillWidth: true
                                        enabled: createBillPanel.payMode === "recurring"
                                        placeholderText: "e.g. 2030"; font.pixelSize: 13
                                        inputMethodHints: Qt.ImhDigitsOnly
                                        validator: IntValidator { bottom: 2000; top: 2100 }
                                        text: (new Date().getFullYear() + 1).toString()
                                        leftPadding: 10; rightPadding: 10; topPadding: 9; bottomPadding: 9
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billScheduleYearField.activeFocus ? 2 : 1
                                            border.color: billScheduleYearField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }
                            }
                        }

                        // ── Assignment card ────────────────────────────────
                        Rectangle {
                            Layout.fillWidth: true
                            // Use implicitHeight (not height) so the outer ColumnLayout
                            // re-measures immediately when the inner content grows/shrinks.
                            implicitHeight: assignCardContent.implicitHeight + 40
                            radius: 10
                            color: RibbonTheme.isDarkMode ? "#1c1c2e" : "#ffffff"
                            border.width: 1
                            border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#e5e7eb"
                            layer.enabled: true
                            layer.effect: RibbonShadow {}

                            ColumnLayout {
                                id: assignCardContent
                                // x/y/width instead of anchors so ColumnLayout.implicitHeight
                                // propagates up synchronously without a deferred layout pass.
                                x: 20; y: 16
                                width: parent.width - 40
                                spacing: 16

                                RowLayout { spacing: 10
                                    Rectangle { width: 28; height: 28; radius: 6; color: "#d97706"
                                        RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Link; iconSize: 14; color: "white" }
                                    }
                                    RibbonText { text: qsTr("Assignment"); font.pixelSize: 14; font.bold: true }
                                }

                                // ASSIGN TO selector
                                // typeKeys / model are FIXED constants — never depend on assignmentTargets
                                // so that API loading never causes a model index shift or spurious
                                // onCurrentIndexChanged that would reset assignmentType back to "none".
                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("ASSIGN TO"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    ComboBox {
                                        id: assignTypeCombo
                                        Layout.fillWidth: true; font.pixelSize: 13; currentIndex: 0
                                        readonly property var typeKeys: ["none","branch","department","property","employee","modification","rental"]
                                        model: ["None","Branch","Department","Property","Employee","Modification","Rental"]
                                        onCurrentIndexChanged: {
                                            var t = typeKeys[currentIndex] || "none"
                                            createBillPanel.assignmentType      = t
                                            createBillPanel.assignmentTargetIdx = 0
                                            createBillPanel.rebuildTargetList(t)
                                        }
                                        contentItem: Text {
                                            leftPadding: 10; text: assignTypeCombo.displayText; font: assignTypeCombo.font
                                            color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"; verticalAlignment: Text.AlignVCenter
                                        }
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: assignTypeCombo.down ? 2 : 1
                                            border.color: assignTypeCombo.down ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                    }
                                }

                                // Conditional target selector — visible when any type selected
                                ColumnLayout {
                                    id: assignTargetSection
                                    spacing: 5; Layout.fillWidth: true
                                    visible: createBillPanel.assignmentType !== "none"

                                    // Label that mirrors the selected type
                                    RibbonText {
                                        text: {
                                            var t = createBillPanel.assignmentType
                                            if (t === "branch")       return qsTr("BRANCH")
                                            if (t === "department")   return qsTr("DEPARTMENT")
                                            if (t === "property")     return qsTr("PROPERTY")
                                            if (t === "employee")     return qsTr("EMPLOYEE")
                                            if (t === "modification") return qsTr("MODIFICATION")
                                            if (t === "rental")       return qsTr("RENTAL")
                                            return ""
                                        }
                                        font.pixelSize: 10; font.bold: true; opacity: 0.45
                                    }

                                    // While API is in flight show a placeholder
                                    RibbonText {
                                        visible: createBillPanel.assignmentTargetsLoading
                                        text: qsTr("Loading...")
                                        font.pixelSize: 11; opacity: 0.5
                                    }

                                    // Target picker — populated once targets are loaded
                                    ComboBox {
                                        id: assignTargetCombo
                                        Layout.fillWidth: true; font.pixelSize: 13
                                        visible: !createBillPanel.assignmentTargetsLoading
                                        model: createBillPanel.targetList
                                        currentIndex: 0
                                        onCurrentIndexChanged: createBillPanel.assignmentTargetIdx = currentIndex
                                        contentItem: Text {
                                            leftPadding: 10; text: assignTargetCombo.displayText; font: assignTargetCombo.font
                                            color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"; verticalAlignment: Text.AlignVCenter
                                        }
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: assignTargetCombo.down ? 2 : 1
                                            border.color: assignTargetCombo.down ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                    }

                                    // "No records found" — only shown once loaded and list is empty
                                    RibbonText {
                                        Layout.fillWidth: true
                                        visible: !createBillPanel.assignmentTargetsLoading
                                                 && createBillPanel.assignmentType !== "none"
                                                 && createBillPanel.targetListEmpty
                                        text: qsTr("No records found. Create one on the web first.")
                                        font.pixelSize: 11; opacity: 0.5; wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }

                        // ── Payment card ───────────────────────────────────
                        Rectangle {
                            Layout.fillWidth: true
                            // implicitHeight drives sizing so dynamic content (amount varies help text)
                            // expands the card without layout deferral issues
                            implicitHeight: payCardContent.implicitHeight + 40
                            radius: 10
                            color: RibbonTheme.isDarkMode ? "#1c1c2e" : "#ffffff"
                            border.width: 1
                            border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#e5e7eb"
                            layer.enabled: true
                            layer.effect: RibbonShadow {}

                            ColumnLayout {
                                id: payCardContent
                                x: 20; y: 16
                                width: parent.width - 40
                                spacing: 16

                                RowLayout { spacing: 10
                                    Rectangle { width: 28; height: 28; radius: 6; color: "#d97706"
                                        RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Payment; iconSize: 14; color: "white" }
                                    }
                                    RibbonText { text: qsTr("Payment"); font.pixelSize: 14; font.bold: true }
                                }

                                // PREFERRED DEBIT ACCOUNT — first (web parity)
                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("PREFERRED DEBIT ACCOUNT  ·  optional"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    ComboBox {
                                        id: billAccountCombo
                                        Layout.fillWidth: true; font.pixelSize: 13; currentIndex: 0
                                        model: {
                                            var list = [qsTr("None — pick when recording each payment")]
                                            for (var i = 0; i < createBillPanel.accountsList.length; i++)
                                                list.push(createBillPanel.accountsList[i].account_name + (createBillPanel.accountsList[i].bank_name ? "  ·  " + createBillPanel.accountsList[i].bank_name : ""))
                                            return list
                                        }
                                        onCurrentIndexChanged: createBillPanel.selectedAccountIdx = currentIndex
                                        contentItem: Text {
                                            leftPadding: 10; text: billAccountCombo.displayText; font: billAccountCombo.font
                                            color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"; verticalAlignment: Text.AlignVCenter
                                        }
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billAccountCombo.down ? 2 : 1
                                            border.color: billAccountCombo.down ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                    }
                                }

                                // AMOUNT (label changes with payment mode)
                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText {
                                        text: createBillPanel.payMode === "recurring"
                                              ? qsTr("AMOUNT  ·  per billing cycle")
                                              : qsTr("PAYMENT AMOUNT  ·  one-time total")
                                        font.pixelSize: 10; font.bold: true; opacity: 0.45
                                    }
                                    TextField {
                                        id: billAmountField
                                        Layout.fillWidth: true
                                        placeholderText: "0.00"; font.pixelSize: 13
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        validator: DoubleValidator { bottom: 0; decimals: 2; notation: DoubleValidator.StandardNotation }
                                        leftPadding: 10; rightPadding: 10; topPadding: 9; bottomPadding: 9
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billAmountField.activeFocus ? 2 : 1
                                            border.color: billAmountField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                    RibbonText {
                                        visible: createBillPanel.amountVaries
                                        Layout.fillWidth: true
                                        text: qsTr("Optional typical amount shown on reminders; the actual total is locked in when you enter it on payment.")
                                        font.pixelSize: 11; opacity: 0.5; wrapMode: Text.WordWrap
                                    }
                                }

                                // AMOUNT VARIES BY USAGE (always visible, web parity)
                                RowLayout {
                                    spacing: 10; Layout.fillWidth: true
                                    Rectangle {
                                        id: amountVariesChk
                                        width: 18; height: 18; radius: 4
                                        color: createBillPanel.amountVaries ? "#4e8ef7" : "transparent"
                                        border.width: createBillPanel.amountVaries ? 0 : 1.5
                                        border.color: RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db"
                                        RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Checkmark; iconSize: 10; color: "white"; visible: createBillPanel.amountVaries }
                                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: createBillPanel.amountVaries = !createBillPanel.amountVaries }
                                    }
                                    ColumnLayout { spacing: 3; Layout.fillWidth: true
                                        RibbonText { text: qsTr("Amount varies by usage or invoice"); font.pixelSize: 12 }
                                        RibbonText {
                                            Layout.fillWidth: true
                                            text: qsTr("For water, utilities, or any bill where each period total is confirmed when you receive the invoice.")
                                            font.pixelSize: 10; opacity: 0.5; wrapMode: Text.WordWrap
                                        }
                                    }
                                }

                                // ALLOW SPLIT PAYMENT (with description, web parity)
                                RowLayout {
                                    spacing: 10; Layout.fillWidth: true
                                    Rectangle {
                                        width: 18; height: 18; radius: 4
                                        color: createBillPanel.splitPayment ? "#4e8ef7" : "transparent"
                                        border.width: createBillPanel.splitPayment ? 0 : 1.5
                                        border.color: RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db"
                                        RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Checkmark; iconSize: 10; color: "white"; visible: createBillPanel.splitPayment }
                                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: createBillPanel.splitPayment = !createBillPanel.splitPayment }
                                    }
                                    ColumnLayout { spacing: 3; Layout.fillWidth: true
                                        RibbonText { text: qsTr("Allow splitting one payment across multiple debit accounts"); font.pixelSize: 12 }
                                        RibbonText {
                                            Layout.fillWidth: true
                                            text: qsTr("Turn off when you always pay each bill from a single account.")
                                            font.pixelSize: 10; opacity: 0.5; wrapMode: Text.WordWrap
                                        }
                                    }
                                }

                                // BILLING CADENCE — always visible, grayed+disabled when one-time (web parity)
                                ColumnLayout {
                                    spacing: 5; Layout.fillWidth: true
                                    opacity: createBillPanel.payMode === "recurring" ? 1.0 : 0.5
                                    RibbonText { text: qsTr("BILLING CADENCE"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    ComboBox {
                                        id: billCadenceCombo
                                        Layout.fillWidth: true; font.pixelSize: 13; currentIndex: 1
                                        enabled: createBillPanel.payMode === "recurring"
                                        model: [qsTr("Per day"), qsTr("Per month"), qsTr("Per year")]
                                        contentItem: Text {
                                            leftPadding: 10; text: billCadenceCombo.displayText; font: billCadenceCombo.font
                                            color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"; verticalAlignment: Text.AlignVCenter
                                        }
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billCadenceCombo.down ? 2 : 1
                                            border.color: billCadenceCombo.down ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                    }
                                }

                                // REMIND BEFORE DAYS (optional)
                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("REMIND BEFORE  ·  optional (days before period end)"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    TextField {
                                        id: billRemindField
                                        Layout.fillWidth: true
                                        placeholderText: qsTr("e.g. 7 — blank for none"); font.pixelSize: 13
                                        inputMethodHints: Qt.ImhDigitsOnly
                                        validator: IntValidator { bottom: 0; top: 366 }
                                        leftPadding: 10; rightPadding: 10; topPadding: 9; bottomPadding: 9
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billRemindField.activeFocus ? 2 : 1
                                            border.color: billRemindField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }

                                // DUE DATE — always visible, hint changes with payment mode (web parity)
                                ColumnLayout {
                                    spacing: 5; Layout.fillWidth: true
                                    RibbonText {
                                        text: createBillPanel.payMode === "one_time"
                                              ? qsTr("DUE DATE  ·  required for one-time (or use first installment date)")
                                              : qsTr("DUE DATE  ·  optional — schedule anchor")
                                        font.pixelSize: 10; font.bold: true; opacity: 0.45
                                    }
                                    TextField {
                                        id: billDueDateField
                                        Layout.fillWidth: true
                                        placeholderText: "YYYY-MM-DD"; font.pixelSize: 13
                                        leftPadding: 10; rightPadding: 10; topPadding: 9; bottomPadding: 9
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billDueDateField.activeFocus ? 2 : 1
                                            border.color: billDueDateField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }

                                // FIRST INSTALLMENT DUE — always visible, alternative date anchor
                                ColumnLayout {
                                    spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("FIRST INSTALLMENT DUE  ·  optional — alternative anchor"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    TextField {
                                        id: billFirstInstallField
                                        Layout.fillWidth: true
                                        placeholderText: "YYYY-MM-DD"; font.pixelSize: 13
                                        leftPadding: 10; rightPadding: 10; topPadding: 9; bottomPadding: 9
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billFirstInstallField.activeFocus ? 2 : 1
                                            border.color: billFirstInstallField.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }
                            }
                        }

                        // ── Notes card ─────────────────────────────────────
                        Rectangle {
                            Layout.fillWidth: true
                            height: notesCardBody.implicitHeight + 40
                            radius: 10
                            color: RibbonTheme.isDarkMode ? "#1c1c2e" : "#ffffff"
                            border.width: 1
                            border.color: RibbonTheme.isDarkMode ? "#2a2a42" : "#e5e7eb"
                            layer.enabled: true
                            layer.effect: RibbonShadow {}

                            ColumnLayout {
                                id: notesCardBody
                                anchors { top: parent.top; left: parent.left; right: parent.right; margins: 20; topMargin: 16 }
                                spacing: 16

                                RowLayout { spacing: 10
                                    Rectangle { width: 28; height: 28; radius: 6; color: "#d97706"
                                        RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.Note; iconSize: 14; color: "white" }
                                    }
                                    RibbonText { text: qsTr("Notes"); font.pixelSize: 14; font.bold: true }
                                }

                                ColumnLayout { spacing: 5; Layout.fillWidth: true
                                    RibbonText { text: qsTr("INTERNAL NOTES"); font.pixelSize: 10; font.bold: true; opacity: 0.45 }
                                    TextArea {
                                        id: billNotesArea
                                        Layout.fillWidth: true
                                        placeholderText: qsTr("Reference numbers or reminders")
                                        font.pixelSize: 13; wrapMode: TextArea.Wrap; height: 80
                                        leftPadding: 10; rightPadding: 10; topPadding: 8; bottomPadding: 8
                                        background: Rectangle {
                                            radius: 6
                                            color: RibbonTheme.isDarkMode ? "#252535" : "#f9fafb"
                                            border.width: billNotesArea.activeFocus ? 2 : 1
                                            border.color: billNotesArea.activeFocus ? "#4e8ef7" : (RibbonTheme.isDarkMode ? "#3a3a5a" : "#d1d5db")
                                        }
                                        color: RibbonTheme.isDarkMode ? "#e0e0f0" : "#111827"
                                    }
                                }
                            }
                        }

                        // ── Save row ───────────────────────────────────────
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.bottomMargin: 4
                            RibbonButton {
                                text: createBillPanel.submitting ? qsTr("Saving…") : qsTr("Save bill")
                                iconSource: RibbonIcons.Checkmark
                                enabled: !createBillPanel.submitting
                                         && billNameField.text.trim() !== ""
                                         && (createBillPanel.amountVaries || billAmountField.text.trim() !== "")
                                         && (createBillPanel.payMode !== "recurring" || billScheduleYearField.text.trim() !== "")
                                         && (createBillPanel.payMode !== "one_time"
                                             || billDueDateField.text.trim() !== ""
                                             || billFirstInstallField.text.trim() !== "")
                                onClicked: {
                                    createBillPanel.statusType = 0
                                    createBillPanel.submitting = true
                                    var catKeys = ["water","electricity","telephone","internet","gas","waste","other"]
                                    var recKeys = ["per_day","per_month","per_year"]
                                    var payload = {
                                        name:                   billNameField.text.trim(),
                                        bill_category:          catKeys[billCategoryCombo.currentIndex],
                                        payment_mode:           createBillPanel.payMode,
                                        recurring_cost:         parseFloat(billAmountField.text) || 0,
                                        amount_varies_by_usage: createBillPanel.amountVaries,
                                        allow_split_payment:    createBillPanel.splitPayment
                                    }
                                    // bill_category_other — send separately, not merged into description
                                    var specText = billSpecifyField.text.trim()
                                    if (billCategoryCombo.currentIndex === 6 && specText !== "")
                                        payload.bill_category_other = specText
                                    // description
                                    var descText = billDescField.text.trim()
                                    if (descText !== "") payload.description = descText
                                    // recurring fields
                                    if (createBillPanel.payMode === "recurring") {
                                        payload.recurring_type = recKeys[billCadenceCombo.currentIndex]
                                        var yr = parseInt(billScheduleYearField.text)
                                        if (!isNaN(yr)) payload.agreement_valid_until_year = yr
                                    }
                                    // dates — send whenever filled; API accepts both for either mode
                                    if (billDueDateField.text.trim() !== "")
                                        payload.due_date = billDueDateField.text.trim()
                                    if (billFirstInstallField.text.trim() !== "")
                                        payload.first_installment_due_date = billFirstInstallField.text.trim()
                                    // remind
                                    var remDays = parseInt(billRemindField.text)
                                    if (!isNaN(remDays) && billRemindField.text.trim() !== "")
                                        payload.remind_before_days = remDays
                                    // debit account
                                    if (createBillPanel.selectedAccountIdx > 0)
                                        payload.deduct_account_id = createBillPanel.accountsList[createBillPanel.selectedAccountIdx - 1].id
                                    // notes
                                    if (billNotesArea.text.trim() !== "") payload.notes = billNotesArea.text.trim()
                                    // assignment
                                    var aType = createBillPanel.assignmentType
                                    payload.assignment_type = aType
                                    if (aType !== "none" && createBillPanel.assignmentTargetIdx > 0) {
                                        var src = aType === "branch"       ? createBillPanel.abBranches
                                                : aType === "department"   ? createBillPanel.abDepartments
                                                : aType === "property"     ? createBillPanel.abProperties
                                                : aType === "employee"     ? createBillPanel.abEmployees
                                                : aType === "modification" ? createBillPanel.abModifications
                                                : aType === "rental"       ? createBillPanel.abRentals
                                                : []
                                        var tgt = src[createBillPanel.assignmentTargetIdx - 1]
                                        if (tgt) {
                                            if (aType === "branch")       payload.branch_id       = tgt.id
                                            if (aType === "department")   payload.department_id   = tgt.id
                                            if (aType === "property")     payload.property_id     = tgt.id
                                            if (aType === "employee")     payload.employee_id     = tgt.id
                                            if (aType === "modification") payload.modification_id = tgt.id
                                            if (aType === "rental") {
                                                payload.rental_id                = tgt.id
                                                payload.rental_property_related  = true
                                            }
                                        }
                                    }
                                    appController.submitBill(payload)
                                }
                            }
                            RibbonText {
                                visible: createBillPanel.statusType === 0
                                text: qsTr("Uses the business selected at login.")
                                font.pixelSize: 11; opacity: 0.4; Layout.fillWidth: true
                            }
                            RibbonText {
                                visible: createBillPanel.statusType !== 0
                                text: createBillPanel.statusMsg
                                color: createBillPanel.statusType === 1 ? "#4caf7d" : "#f74e6c"
                                font.pixelSize: 12; Layout.fillWidth: true; wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
    }

    // ── Status bar ─────────────────────────────────────────────────────────
    RibbonBottomBar {
        id: statusBar
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        RibbonButton { showBg: false; adaptHeight: true; iconSource: RibbonIcons.Info; text: qsTr("Zeebroo POS  ·  v1.0  ·  Ready") }
        rightContent: RowLayout {
            spacing: 4; Layout.preferredHeight: parent.height; layoutDirection: Qt.RightToLeft
            RibbonButton { showBg: false; adaptHeight: true; iconSource: RibbonIcons.Settings; tipText: qsTr("Settings"); onClicked: settingsView.open(); visible: false }
        }
    }

    titleBar.titleIconSource: "qrc:/logo.png"

    // ── Backstage ──────────────────────────────────────────────────────────
    RibbonBackStageView {
        id: settingsView
        implicitHeight: root.height - root.borderWidth * 2
        implicitWidth:  root.width  - root.borderWidth * 2
        blurEnabled: true
        blurTarget: root.windowItems

        RibbonBackStageMenuItem {
            menuText: qsTr("Expenses"); menuIcon: RibbonIcons.MoneyDismiss
            type: RibbonBackStageView.MenuItemLocation.Head; sourceComponent: expensesPage
        }
        RibbonBackStageMenuItem {
            menuText: qsTr("Income"); menuIcon: RibbonIcons.MoneyHand
            type: RibbonBackStageView.MenuItemLocation.Head; sourceComponent: incomePage
        }
        RibbonBackStageMenuItem {
            menuText: qsTr("Sales"); menuIcon: RibbonIcons.ChartMultiple
            type: RibbonBackStageView.MenuItemLocation.Head; sourceComponent: salesPage
        }
        RibbonBackStageMenuItem {
            menuText: qsTr("Close"); menuIcon: RibbonIcons.ArrowLeft
            type: RibbonBackStageView.MenuItemLocation.Tail
            clickOnly: true; clickFunc: () => settingsView.close()
        }
    }

    // ── Expenses page ──────────────────────────────────────────────────────
    Component {
        id: expensesPage
        RibbonBackStagePage {
            pageName: qsTr("Expenses")
            GridView {
                anchors { fill: parent; margins: 12 }
                cellWidth: 180; cellHeight: 200; clip: true
                model: [
                    { label: qsTr("Create a Bill"),   desc: qsTr("Record a new payable bill"),          icon: RibbonIcons.Document,       accent: "#4e8ef7", action: "createBill" },
                    { label: qsTr("Loan Setup"),      desc: qsTr("Configure loan terms & repayments"),  icon: RibbonIcons.MoneyCalculator, accent: "#9c6ef7" },
                    { label: qsTr("Make Rental"),     desc: qsTr("Log rental or lease payments"),       icon: RibbonIcons.Home,            accent: "#f7a54e" },
                    { label: qsTr("Employee Salary"), desc: qsTr("Process staff salary payments"),      icon: RibbonIcons.PersonMoney,     accent: "#4caf7d" },
                    { label: qsTr("Modification"),    desc: qsTr("Track repair & upgrade costs"),       icon: RibbonIcons.Wrench,          accent: "#f0a030" },
                    { label: qsTr("Purchase Order"),  desc: qsTr("Raise a supplier purchase order"),    icon: RibbonIcons.ShoppingBag,     accent: "#4ee0f7" },
                    { label: qsTr("Legal"),           desc: qsTr("Legal fees & compliance costs"),      icon: RibbonIcons.Scales,          accent: "#f74e6c" },
                    { label: qsTr("Transport"),       desc: qsTr("Freight, fuel & travel expenses"),    icon: RibbonIcons.Airplane,        accent: "#60c060" },
                    { label: qsTr("Marketing"),       desc: qsTr("Ads, promotions & campaigns"),        icon: RibbonIcons.Megaphone,       accent: "#e040fb" },
                    { label: qsTr("Other Expenses"),  desc: qsTr("Miscellaneous uncategorised costs"),  icon: RibbonIcons.Calculator,      accent: "#90a4ae" },
                ]
                delegate: Item {
                    required property var modelData
                    width: GridView.view.cellWidth; height: GridView.view.cellHeight
                    Rectangle {
                        anchors { fill: parent; margins: 8 }
                        radius: 12
                        color: RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                        border.width: 1; border.color: RibbonTheme.isDarkMode ? "#3a3a4a" : "#e8eaf0"
                        layer.enabled: true; layer.effect: RibbonShadow {}
                        ColumnLayout {
                            anchors { fill: parent; margins: 14 }
                            spacing: 8
                            Rectangle {
                                Layout.alignment: Qt.AlignHCenter; Layout.topMargin: 4
                                width: 52; height: 52; radius: 13; color: modelData.accent
                                RibbonIcon { anchors.centerIn: parent; iconSource: modelData.icon; iconSize: 26; color: "white" }
                            }
                            RibbonText { Layout.alignment: Qt.AlignHCenter; Layout.fillWidth: true; text: modelData.label; font.pixelSize: 12; font.bold: true; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
                            RibbonText { Layout.alignment: Qt.AlignHCenter; Layout.fillWidth: true; text: modelData.desc; font.pixelSize: 10; opacity: 0.55; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor; hoverEnabled: true
                            onEntered: parent.color = RibbonTheme.isDarkMode ? "#303045" : "#eef2ff"
                            onExited:  parent.color = RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            onClicked: {
                                if (typeof modelData.action !== "undefined" && modelData.action === "createBill") {
                                    settingsView.close()
                                    contentArea.createBillTabOpen = true
                                    contentArea.activeTab = "createBill"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ── Income page ────────────────────────────────────────────────────────
    Component {
        id: incomePage
        RibbonBackStagePage {
            pageName: qsTr("Income")
            GridView {
                anchors { fill: parent; margins: 12 }
                cellWidth: 180; cellHeight: 200; clip: true
                model: [
                    { label: qsTr("Create Quotation"),    desc: qsTr("Generate a price quote for a customer"),  icon: RibbonIcons.DocumentPercent, accent: "#4e8ef7" },
                    { label: qsTr("Create Lead"),          desc: qsTr("Add a new sales lead or prospect"),       icon: RibbonIcons.PersonStar,      accent: "#9c6ef7" },
                    { label: qsTr("POS"),                  desc: qsTr("Open point-of-sale for quick sales"),     icon: RibbonIcons.Cart,            accent: "#4caf7d", action: "openPos" },
                    { label: qsTr("Create Sales Invoice"), desc: qsTr("Issue an invoice to a customer"),         icon: RibbonIcons.ReceiptAdd,       accent: "#f7a54e" },
                    { label: qsTr("Credit Recovery"),      desc: qsTr("Track & recover outstanding credits"),    icon: RibbonIcons.CreditCardPerson, accent: "#f74e6c" },
                    { label: qsTr("Profit Analytics"),     desc: qsTr("View revenue trends & profit reports"),   icon: RibbonIcons.DataTrending,    accent: "#e040fb" },
                ]
                delegate: Item {
                    required property var modelData
                    width: GridView.view.cellWidth; height: GridView.view.cellHeight
                    Rectangle {
                        anchors { fill: parent; margins: 8 }
                        radius: 12
                        color: RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                        border.width: 1; border.color: RibbonTheme.isDarkMode ? "#3a3a4a" : "#e8eaf0"
                        layer.enabled: true; layer.effect: RibbonShadow {}
                        ColumnLayout {
                            anchors { fill: parent; margins: 14 }
                            spacing: 8
                            Rectangle {
                                Layout.alignment: Qt.AlignHCenter; Layout.topMargin: 4
                                width: 52; height: 52; radius: 13; color: modelData.accent
                                RibbonIcon { anchors.centerIn: parent; iconSource: modelData.icon; iconSize: 26; color: "white" }
                            }
                            RibbonText { Layout.alignment: Qt.AlignHCenter; Layout.fillWidth: true; text: modelData.label; font.pixelSize: 12; font.bold: true; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
                            RibbonText { Layout.alignment: Qt.AlignHCenter; Layout.fillWidth: true; text: modelData.desc; font.pixelSize: 10; opacity: 0.55; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor; hoverEnabled: true
                            onEntered: parent.color = RibbonTheme.isDarkMode ? "#303045" : "#eef2ff"
                            onExited:  parent.color = RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            onClicked: {
                                if (typeof modelData.action !== "undefined" && modelData.action === "openPos") {
                                    settingsView.close()
                                    appController.openPos()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ── Sales page ─────────────────────────────────────────────────────────
    Component {
        id: salesPage
        RibbonBackStagePage {
            pageName: qsTr("Sales")
            ColumnLayout {
                anchors { top: parent.top; left: parent.left; right: parent.right; margins: 8 }
                spacing: 16
                RibbonBackStageGroup {
                    Layout.fillWidth: true; height: 200; groupName: qsTr("Recent Sales")
                    ColumnLayout {
                        anchors { fill: parent; topMargin: 8 }
                        spacing: 8
                        Repeater {
                            model: [
                                { label: qsTr("Receipt #1042"), amount: "245.00", date: "2025-06-10" },
                                { label: qsTr("Receipt #1043"), amount: "87.50",  date: "2025-06-10" },
                                { label: qsTr("Receipt #1044"), amount: "310.00", date: "2025-06-11" },
                            ]
                            delegate: RowLayout {
                                required property var modelData
                                Layout.fillWidth: true
                                RibbonIcon { iconSource: RibbonIcons.Receipt; iconSize: 16; color: "#4e8ef7" }
                                RibbonText { text: modelData.label; Layout.fillWidth: true; font.pixelSize: 13 }
                                RibbonText { text: modelData.date; font.pixelSize: 11; opacity: 0.6 }
                                RibbonText { text: "$ " + modelData.amount; font.pixelSize: 13; font.bold: true; color: "#4e8ef7" }
                            }
                        }
                        RibbonButton { Layout.topMargin: 4; text: qsTr("View All Sales"); iconSource: RibbonIcons.ChartMultiple }
                    }
                }
            }
        }
    }
}
