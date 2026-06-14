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
        property bool   createBillTabOpen: false
        property string activeTab:         "flowchart"   // "flowchart" | "createBill" | ""

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
                                    contentArea.activeTab = contentArea.createBillTabOpen ? "createBill" : ""
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

                // "Reopen" button when all tabs closed
                RibbonButton {
                    visible: !contentArea.flowchartTabOpen && !contentArea.createBillTabOpen
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

                // Sub-node 3×3 grid (118×36 cells, 6px col-gap, 8px row-gap)
                readonly property real subCellW:  118
                readonly property real subCellH:  36
                readonly property real subColGap: 6
                readonly property real subRowGap: 8
                property real gridW:    3 * subCellW + 2 * subColGap   // 366
                property real gridH:    3 * subCellH + 2 * subRowGap   // 124
                property real gridRight: expNodeX - 84
                property real gridLeft:  gridRight - gridW
                property real gridTop:   expNodeY - gridH / 2

                property var mainNodes: [
                    { idx: 0, label: qsTr("POS"),       icon: RibbonIcons.Cart,          accent: "#4caf7d", angle: -90,  action: "openPos" },
                    { idx: 1, label: qsTr("Inventory"), icon: RibbonIcons.Box,           accent: "#4e8ef7", angle: -30,  action: "" },
                    { idx: 2, label: qsTr("Customers"), icon: RibbonIcons.People,        accent: "#9c6ef7", angle:  30,  action: "" },
                    { idx: 3, label: qsTr("Reports"),   icon: RibbonIcons.ChartMultiple, accent: "#f7a54e", angle:  90,  action: "" },
                    { idx: 4, label: qsTr("Income"),    icon: RibbonIcons.MoneyHand,     accent: "#60c060", angle:  150, action: "" },
                    { idx: 5, label: qsTr("Expenses"),  icon: RibbonIcons.MoneyDismiss,  accent: "#f74e6c", angle: -150, action: "" },
                ]

                // idx indexes into subNodeCenters; row/col set the initial grid position
                property var expenseSubNodes: [
                    { idx: 0, row: 0, col: 0, label: qsTr("Bill"),             icon: RibbonIcons.Document,        accent: "#4e8ef7", action: "openBill" },
                    { idx: 1, row: 0, col: 1, label: qsTr("Loan Setup"),       icon: RibbonIcons.MoneyCalculator, accent: "#9c6ef7", action: "" },
                    { idx: 2, row: 0, col: 2, label: qsTr("Rental"),           icon: RibbonIcons.Home,            accent: "#f7a54e", action: "" },
                    { idx: 3, row: 1, col: 0, label: qsTr("Employee Salary"),  icon: RibbonIcons.PersonMoney,     accent: "#4caf7d", action: "" },
                    { idx: 4, row: 1, col: 1, label: qsTr("Modification"),     icon: RibbonIcons.Wrench,          accent: "#f0a030", action: "" },
                    { idx: 5, row: 1, col: 2, label: qsTr("Purchase Order"),   icon: RibbonIcons.ShoppingBag,     accent: "#4ee0f7", action: "" },
                    { idx: 6, row: 2, col: 0, label: qsTr("Legal"),            icon: RibbonIcons.Scales,          accent: "#f74e6c", action: "" },
                    { idx: 7, row: 2, col: 1, label: qsTr("Marketing"),        icon: RibbonIcons.Megaphone,       accent: "#e040fb", action: "" },
                    { idx: 8, row: 2, col: 2, label: qsTr("Transport"),        icon: RibbonIcons.Airplane,        accent: "#60c060", action: "" },
                ]

                // Delegates write here on every move; canvas reads these for line endpoints
                property var mainNodeCenters: [Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0)]
                property var subNodeCenters:  [Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0)]

                // Live loans fetched from API — displayed as leaf nodes under Loan Setup (subIdx=1)
                property var loanNodes: []
                readonly property real lnW:   subCellW
                readonly property real lnH:   subCellH
                readonly property real lnGap: 6
                // Initial column position mirrors Loan Setup column (col=1) in the expense grid
                property real lnNodeX:    gridLeft + 1 * (subCellW + subColGap)
                property real lnNodeYBase: gridTop - Math.max(1, loanNodes.length) * (lnH + lnGap) - 14

                Connections {
                    target: appController
                    function onLoansLoaded(loans) {
                        flowChart.loanNodes = loans
                        lineCanvas.requestPaint()
                    }
                }
                Component.onCompleted: Qt.callLater(function() { appController.fetchLoans() })

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
                        var lineCol  = RibbonTheme.isDarkMode ? "#3a3a5a" : "#c0cce0"
                        var arrowCol = RibbonTheme.isDarkMode ? "#5a6a9a" : "#8898c8"
                        var hubR = 56, nodeR = 22

                        // Hub → each main node using actual (possibly dragged) center positions
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
                            ctx.strokeStyle = lineCol; ctx.lineWidth = 1.5; ctx.setLineDash([6, 5])
                            ctx.beginPath(); ctx.moveTo(x1, y1); ctx.lineTo(x2, y2); ctx.stroke()
                            ctx.setLineDash([]); ctx.fillStyle = arrowCol
                            var sz = 7, aa = Math.atan2(y2 - y1, x2 - x1)
                            ctx.beginPath()
                            ctx.moveTo(x2, y2)
                            ctx.lineTo(x2 - sz * Math.cos(aa - 0.42), y2 - sz * Math.sin(aa - 0.42))
                            ctx.lineTo(x2 - sz * Math.cos(aa + 0.42), y2 - sz * Math.sin(aa + 0.42))
                            ctx.closePath(); ctx.fill()
                        }

                        // Expenses node → each expense sub-node (thin dashed, tracks drag)
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
                                ctx.strokeStyle = lineCol; ctx.lineWidth = 1; ctx.setLineDash([4, 4])
                                ctx.beginPath()
                                ctx.moveTo(expC.x + snx * nodeR, expC.y + sny * nodeR)
                                ctx.lineTo(sc.x - snx * 20,      sc.y - sny * 20)
                                ctx.stroke()
                                ctx.setLineDash([])
                            }
                        }

                        // Loan Setup sub-node (subIdx=1) → each loan node
                        var loanNodes = flowChart.loanNodes
                        if (loanNodes.length > 0) {
                            var lsC = flowChart.subNodeCenters[1]
                            if (lsC && (lsC.x !== 0 || lsC.y !== 0)) {
                                var lnCx = flowChart.lnNodeX + flowChart.lnW / 2
                                for (var k = 0; k < loanNodes.length; k++) {
                                    var lnCy = flowChart.lnNodeYBase + k * (flowChart.lnH + flowChart.lnGap) + flowChart.lnH / 2
                                    var ldx = lnCx - lsC.x, ldy = lnCy - lsC.y
                                    var ldist = Math.sqrt(ldx * ldx + ldy * ldy)
                                    if (ldist < 1) continue
                                    var lnx = ldx / ldist, lny = ldy / ldist
                                    ctx.strokeStyle = "#9c6ef7"; ctx.lineWidth = 1; ctx.setLineDash([3, 4])
                                    ctx.beginPath()
                                    ctx.moveTo(lsC.x + lnx * 18, lsC.y + lny * 18)
                                    ctx.lineTo(lnCx - lnx * 18,  lnCy - lny * 18)
                                    ctx.stroke()
                                    ctx.setLineDash([])
                                }
                            }
                        }
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
                            x = flowChart.gridLeft + modelData.col * (flowChart.subCellW + flowChart.subColGap)
                            y = flowChart.gridTop  + modelData.row * (flowChart.subCellH + flowChart.subRowGap)
                        }
                        Component.onCompleted: {
                            _initPos()
                            Qt.callLater(function() { subNode._ready = true })
                        }
                        Connections {
                            target: flowChart
                            function onGridLeftChanged() { if (!subNode._moved) subNode._initPos() }
                            function onGridTopChanged()  { if (!subNode._moved) subNode._initPos() }
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
                                    contentArea.createBillTabOpen = true
                                    contentArea.activeTab = "createBill"
                                }
                            }
                        }
                    }
                }

                // Loan nodes — fetched from API, displayed above Loan Setup (expense sub-node col=1)
                Repeater {
                    model: flowChart.loanNodes
                    delegate: Item {
                        id: lnNode
                        required property var modelData
                        required property int index
                        width: flowChart.lnW; height: flowChart.lnH
                        z: 3
                        x: flowChart.lnNodeX
                        y: flowChart.lnNodeYBase + index * (flowChart.lnH + flowChart.lnGap)
                        onXChanged: lineCanvas.requestPaint()
                        onYChanged: lineCanvas.requestPaint()

                        Rectangle {
                            anchors.fill: parent; radius: 7
                            color: RibbonTheme.isDarkMode ? "#252535" : "#ffffff"
                            border.width: 1.5; border.color: "#9c6ef7"
                            layer.enabled: true; layer.effect: RibbonShadow {}
                            RowLayout {
                                anchors { fill: parent; leftMargin: 7; rightMargin: 5; topMargin: 3; bottomMargin: 3 }
                                spacing: 6
                                Rectangle {
                                    width: 22; height: 22; radius: 5; color: "#9c6ef7"
                                    RibbonIcon { anchors.centerIn: parent; iconSource: RibbonIcons.MoneyCalculator; iconSize: 11; color: "white" }
                                }
                                RibbonText { text: lnNode.modelData.name; font.pixelSize: 10; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                            }
                        }
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
