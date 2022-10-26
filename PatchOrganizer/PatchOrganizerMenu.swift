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
    
    init(cv: ContentView) {
        self.cv = cv
    }
    
    var body: some Commands {
        //EmptyCommands()

        CommandGroup(before: .newItem) {
            Button("Reload") {
                cv.actionReload()
            }.keyboardShortcut("O", modifiers: .command)
        }
        CommandGroup(before: .newItem) {
            Button("Open...") {
                cv.actionOpen()
            }.keyboardShortcut("R", modifiers: .command)
        }
        CommandGroup(before: .importExport) {
            Button("Export...") {
                cv.actionExport()
            }.keyboardShortcut("E", modifiers: .command)
        }
    }
}