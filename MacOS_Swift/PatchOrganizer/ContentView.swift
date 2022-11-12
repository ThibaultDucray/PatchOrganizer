//  ContentView.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 17/10/2022.
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

class MultiSel: ObservableObject {
    @Published var multiSelection = Set<Int>()
}

struct ContentView: View {
    @Environment(\.openURL) var openURL
    @ObservedObject var ms = MultiSel()
    @ObservedObject var version = VersionControler()
    @ObservedObject var uiPatches = UIPatches()
    
    var body: some View {
        
        VStack {
            Text("Patches list")
            List (selection: $ms.multiSelection) {
                
                ForEach($uiPatches.patches, id: \.id) { $p in
                    HStack {
                        Label("\(p.bank).\(p.num)\t", systemImage: "music.note")
                        Image(systemName: p.userIR ? "circle.fill.square.fill" : "circle.square")
                            .foregroundColor(.accentColor)
                        TextField("", text: $p.name)
                    }
                }
                .onMove (perform: uiPatches.actionMove)
            }
        }
        .alert("Please open a valid preset file.", isPresented: $uiPatches.emptyError, actions: {
            Button("OK", role: .cancel, action: {})
            })
        .alert("Could not save as \(uiPatches.newFileName ?? "none")\nError code \(uiPatches.errcode)", isPresented: $uiPatches.saveError, actions: {
            Button("Cancel", role: .cancel, action: {})
            })
        .alert("Could not open \(uiPatches.actualFileName ?? "none")", isPresented: $uiPatches.openError, actions: {
            Button("OK", role: .cancel, action: {})
            })
        .alert("Can only replace by file containing 1 patch", isPresented: $uiPatches.replaceError, actions: {
            Button("OK", role: .cancel, action: {})
            })
        .alert("New version available!\nActual: \(version.actualVersion)\nNew: \(version.distantVersion ?? "new")", isPresented: $version.newVersion, actions: {
            Button("Cancel", role: .cancel, action: {})
            Button("Open website", role: .cancel, action: {
                openURL(URL(string: "https://github.com/ThibaultDucray/PatchOrganizer/releases/")!)
            })

        })
        .navigationTitle("Ampero Patch Organizer")
        .refreshable {
        }
        .onAppear(perform: {
            version.control()
        })
        
        
        Text("Selected \(ms.multiSelection.count) lines")
        
        HStack {
            Button(action: actionReplace) {
                Label("Replace", systemImage: "arrow.backward.circle")
            } .disabled(ms.multiSelection.count != 1)
            Button(action: actionExport) {
                Label("Export", systemImage: "arrow.forward.circle")
            } .disabled(ms.multiSelection.count != 1)
            Button(action: uiPatches.actionReload) {
                Label("Reload", systemImage: "arrow.counterclockwise")
            }
            Button(action: uiPatches.actionOpen) {
                Label("Open...", systemImage: "doc")
            }
            Button(action: uiPatches.actionSaveAs) {
                Label("Save as...", systemImage: "arrow.down.doc")
            }
        }
        .buttonStyle(.bordered)
        
        Text("File: \(uiPatches.actualFileName ?? "none")")
    }

    func actionReplace() {
        if ms.multiSelection.count == 1 {
            uiPatches.actionReplace(index: ms.multiSelection.first ?? 0)
        }
    }
    
    func actionExport() {
        if ms.multiSelection.count == 1 {
            uiPatches.exportOnePatch(index: ms.multiSelection.first ?? 0)
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
