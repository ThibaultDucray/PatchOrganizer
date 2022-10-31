//
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

class UIPatch: Identifiable, ObservableObject {
    let id: Int
    @Published var bank: Int
    @Published var num: Int
    @Published var userIR: Bool
    @Published var name: String
    
    static func == (lhs: UIPatch, rhs: UIPatch) -> Bool {
        return (lhs.bank == rhs.bank) && (lhs.num == rhs.num) && (lhs.name == rhs.name)
    }
    
    init(id: Int, elem: (bank: Int, num: Int, userIR: Bool, name:String)) {
        self.bank = elem.bank
        self.num = elem.num
        self.userIR = elem.userIR
        self.name = elem.name
        self.id = id //elem.bank * 3 + elem.num
    }
}

class UIPatches: ObservableObject {
    @Published var patches: [UIPatch] = []
    @Published var actualFileName : String?
    var newFileName : String?
    @Published var invertTailBit:Bool = false
    @Published var openError = false
    @Published var emptyError = false
    @Published var saveError = false
    var errcode = 0
    var patchHandler: PatchHandler?
    
    func readFromFile(fileName: String) {
        self.actualFileName = fileName
        openError = false
        patchHandler = PatchHandler(fileName: fileName)
        if let vf = patchHandler?.validFile {
            if (vf) {
                // todo? make this func async
                patches = []
                if let nbPatches = patchHandler?.nbPatches {
                    for i in (0 ... nbPatches - 1) {
                        if let elem = patchHandler?.getElem(i: Int(i)) {
                            let p = UIPatch(id: Int(i), elem: elem)
                            patches.append(p)
                        }
                    }
                }
                patches.sort(by: {(a:UIPatch, b:UIPatch) -> Bool in return (a.bank * 3 + a.num) < (b.bank * 3 + b.num) })
            } else {
                openError = true
            }
        } else {
            openError = true
        }
    }
    
    func reReadFromFile() {
        if let fn = self.actualFileName {
            readFromFile(fileName: fn)
        }
    }
    
    func writeToFile(newFileName: String) {
        saveError = false
        self.newFileName = newFileName

        if let nbPatches = patchHandler?.nbPatches {
            for i in (0 ... nbPatches - 1) {
                let id = patches[Int(i)].id
                let bank = patches[Int(i)].bank
                let num = patches[Int(i)].num
                let name = patches[Int(i)].name
                patchHandler?.setElem(actualpos: id, bank: bank, num: num, name: name, newpos: Int(i))
            }
        }
        if let err = patchHandler?.writePatchlist(fileName: newFileName, invertTailBit: invertTailBit) {
            self.errcode = err
            saveError = (err <= 0)
        }
    }
}

struct ContentView: View {
    @State private var multiSelection = Set<Int>()
    @ObservedObject var uiPatches = UIPatches()
    
    var body: some View {
        
        VStack {
            Text("Patches list")
            List (selection: $multiSelection) {
                
                ForEach($uiPatches.patches, id: \.id) { $p in
                    HStack {
                        Label("\(p.bank).\(p.num)\t", systemImage: "music.note")
                        Image(systemName: p.userIR ? "circle.fill.square.fill" : "circle.square")
                            .foregroundColor(.accentColor)
                        TextField("", text: $p.name)
                    }
                }
                .onMove (perform: move)
            }
        }
        .alert("Please open a valid preset file.", isPresented: $uiPatches.emptyError, actions: {
            Button("OK", role: .cancel, action: {})
            })
        .alert("Could not save as \(uiPatches.newFileName ?? "none")\nError code \(uiPatches.errcode)", isPresented: $uiPatches.saveError, actions: {
            Button("OK", role: .cancel, action: {})
            })
        .alert("Could not open \(uiPatches.actualFileName ?? "none")", isPresented: $uiPatches.openError, actions: {
            Button("OK", role: .cancel, action: {})
            })
        .navigationTitle("Ampero Patch Organizer")
        .refreshable {
        }
        
        
        Text("Selected \(multiSelection.count) lines")
        
        HStack {
            Button(action: actionReload) {
                Label("Reload", systemImage: "arrow.counterclockwise")
            }
            Button(action: actionOpen) {
                Label("Open...", systemImage: "doc")
            }
            Button(action: actionSaveAs) {
                Label("Save as...", systemImage: "arrow.down.doc")
            }
        }
        .buttonStyle(.bordered)
        
        Text("File: \(uiPatches.actualFileName ?? "none")")
    }

        
    func move(from source: IndexSet, to destination: Int) {
        uiPatches.patches.move(fromOffsets: source, toOffset: destination)
    }
        
    func actionOpen() {
        let panel = NSOpenPanel()
        panel.title = "Open patches list"
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        //panel.allowedContentTypes = [UTType(filenameExtention: "prst")]
        if panel.runModal() == .OK {
            uiPatches.readFromFile(fileName: panel.url?.path ?? "<none>")
        }
    }
    func actionSaveAs() {
        if uiPatches.actualFileName == nil {
            uiPatches.emptyError = true
            return
        }
        let panel = NSSavePanel() //NSOpenPanel()
        panel.title = "Save patches list"
        if panel.runModal() == .OK {
            uiPatches.writeToFile(newFileName: panel.url?.path ?? "<none>")
        }
    }
    
    func actionReload() {
        if uiPatches.actualFileName == nil {
            uiPatches.emptyError = true
            return
        }
        uiPatches.reReadFromFile()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
