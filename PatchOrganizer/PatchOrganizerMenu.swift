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
    var cv: ContentView
    @Environment(\.openURL) var openURL
    @ObservedObject var uiPatches: UIPatches
    
    init(cv: ContentView) {
        self.cv = cv
        uiPatches = cv.uiPatches
    }

    var body: some Commands {
        EmptyCommands()

        CommandGroup(before: .newItem) {
            Button("Open...") {
                cv.actionOpen()
            }.keyboardShortcut("O", modifiers: .command)
            
            Button("Reload") {
                cv.actionReload()
            }.keyboardShortcut("R", modifiers: .command)
        }

        CommandGroup(before: .saveItem) {
            Button("Save as...") {
                cv.actionSaveAs()
            }.keyboardShortcut("S", modifiers: .command)
            Picker("Tail bit (beta)", selection: $uiPatches.invertTailBit) {
                Text("Normal tail bit").tag(false)
                Text("Inverted tail bit").tag(true)
            }
        }
        
        CommandGroup(replacing: .help, addition: {
            Button("PatchOrganizer Help") {
                openURL(URL(string: "https://github.com/ThibaultDucray/PatchOrganizer/wiki")!)
            }
        })
            
        
    }
}
