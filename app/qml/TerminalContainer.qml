/*******************************************************************************
* Copyright (c) 2013-2021 "Filippo Scognamiglio"
* https://github.com/Swordfish90/cool-retro-term
*
* This file is part of cool-retro-term.
*
* cool-retro-term is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
import QtQuick 2.2
import QtQuick.Effects

import "utils.js" as Utils

ShaderTerminal {
    property alias title: terminal.title
    property alias terminalSize: terminal.terminalSize
    property bool isActive: false
    signal sessionFinished()

    property bool loadBloomEffect: appSettings.bloom > 0 || appSettings._frameShininess > 0

    id: mainShader
    opacity: appSettings.windowOpacity * 0.3 + 0.7

    source: terminal.mainSource
    burnInEffect: terminal.burnInEffect
    virtualResolution: terminal.virtualResolution
    screenResolution: Qt.size(
        terminalWindow.width * Screen.devicePixelRatio * appSettings.windowScaling,
        terminalWindow.height * Screen.devicePixelRatio * appSettings.windowScaling
    )
    bloomSource: bloomSourceLoader.item

    PreprocessedTerminal {
        id: terminal
        anchors.fill: parent
        isActive: mainShader.isActive
        onSessionFinished: mainShader.sessionFinished()
    }

    function activate() {
        terminal.mainTerminal.forceActiveFocus()
    }

    //  EFFECTS  ////////////////////////////////////////////////////////////////
    Loader {
        id: bloomEffectLoader
        active: loadBloomEffect
        asynchronous: true
        width: parent.width * appSettings.bloomQuality
        height: parent.height * appSettings.bloomQuality

        sourceComponent: MultiEffect {
            source: terminal.mainSource
            blurEnabled: true
            blur: 1.0
            blurMax: Math.round(Utils.lint(16, 64, appSettings.bloomQuality))
            // Padding would make MultiEffect re-layer the source, and
            // mainSource is an invisible ShaderEffectSource, so it must be
            // sampled directly as a texture provider.
            autoPaddingEnabled: false
        }
    }
    Loader {
        id: bloomSourceLoader
        active: loadBloomEffect
        asynchronous: true
        sourceComponent: ShaderEffectSource {
            id: _bloomEffectSource
            sourceItem: bloomEffectLoader.item
            wrapMode: ShaderEffectSource.Repeat
            hideSource: true
            smooth: true
            visible: false
        }
    }
}
