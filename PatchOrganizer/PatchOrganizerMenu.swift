//
//  PatchOrganizerMenu.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 25/10/2022.
//
// This file is part of PatchOrganizer.
//
// PatchOrganizer is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// PatchOrganizer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with PatchOrganizer.
// If not, see <https://www.gnu.org/licenses/>.
//
// (c) Copyright 2022 Thibault Ducray


import SwiftUI

struct Menus: Commands {
    @Environment(\.openURL) var openURL
    @ObservedObject var uiPatches: UIPatches
    @ObservedObject var ms: MultiSel

    var body: some Commands {
        EmptyCommands()

        CommandGroup(before: .newItem) {
            Button("Open...") {
                uiPatches.actionOpen()
            }.keyboardShortcut("O", modifiers: .command)
            
            Button("Reload") {
                uiPatches.actionReload()
            }.keyboardShortcut("L", modifiers: .command)
        }

        CommandGroup(before: .saveItem) {
            Button("Save as...") {
                uiPatches.actionSaveAs()
            }.keyboardShortcut("S", modifiers: .command)
            Picker("Tail bit (beta)", selection: $uiPatches.invertTailBit) {
                Text("Normal tail bit").tag(false)
                Text("Inverted tail bit").tag(true)
            }
        }
        
        CommandMenu("Patch") {
            Button("Export 1 patch") {
                uiPatches.exportOnePatch(index: ms.multiSelection.first ?? 0)
            }
            .disabled(ms.multiSelection.count != 1)
            .keyboardShortcut("E", modifiers: .command)
            Button("Replace 1 patch") {
                uiPatches.actionReplace(index: ms.multiSelection.first ?? 0)
            }
            .disabled(ms.multiSelection.count != 1)
            .keyboardShortcut("R", modifiers: .command)
        }
        
        // help opens url on github
        CommandGroup(replacing: .help, addition: {
            Button("PatchOrganizer Help") {
                openURL(URL(string: "https://github.com/ThibaultDucray/PatchOrganizer/wiki")!)
            }
        })
    }
}
