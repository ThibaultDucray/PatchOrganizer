//
//  ContentView.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 17/10/2022.
//

import SwiftUI

class UIPatch: Identifiable, ObservableObject {
    let id: Int
    @Published var bank: Int
    @Published var num: Int
    @Published var name: String
    
    static func == (lhs: UIPatch, rhs: UIPatch) -> Bool {
        return (lhs.bank == rhs.bank) && (lhs.num == rhs.num) && (lhs.name == rhs.name)
    }
    
    init(id: Int, elem: (bank: Int, num: Int, name:String)) {
        self.bank = elem.bank
        self.num = elem.num
        self.name = elem.name
        self.id = id //elem.bank * 3 + elem.num
    }
    
//    init(id: Int, bank: Int, num: Int, name: String) {
//        self.id = id
//        self.bank = bank
//        self.num = num
//        self.name = name
//    }
}

class UIPatches: ObservableObject {
    @Published var patches = dummypatches
    @Published var fileName = "<nofilename>"

    func readFromFile(fileName: String) {
        self.fileName = fileName
        let patchHandler = PatchHandler(fileName: fileName)

        // todo: make this func async
        patches = []
        for i in (0 ... NBPATCHES - 1) {
            let p = UIPatch(id: Int(i), elem: patchHandler.getElem(i: Int(i)))
            patches.append(p)
        }
        patches.sort(by: {(a:UIPatch, b:UIPatch) -> Bool in return (a.bank * 3 + a.num) < (b.bank * 3 + b.num) })
    }
    
    func reReadFromFile() {
        readFromFile(fileName: fileName)
    }
    
    func writeToFile(fileName: String) {
        let patchHandler = PatchHandler(fileName: self.fileName)
        
        for i in (0 ... NBPATCHES - 1) {
            let id = patches[Int(i)].id
            let bank = patches[Int(i)].bank
            let num = patches[Int(i)].num
            let name = patches[Int(i)].name
            patchHandler.setElem(actualpos: id, bank: bank, num: num, name: name, newpos: Int(i))
        }
        let err = patchHandler.writePatchlist(fileName: fileName)
        if (err <= 0) {
            // TODO (does not show)
            //Alert(title: Text("Could not write file \(fileName)"))
        } else {
            //Alert(title: Text("Presets exported to \(fileName)"))
        }
    }
}

struct ContentView: View {
    @State private var multiSelection = Set<Int>()
    @ObservedObject var patches = UIPatches()
    
    var body: some View {
        
        VStack {
            Text("Patches list")
            List (selection: $multiSelection) {
                
                ForEach(patches.patches, id: \.id) { patch in
                    //Text("\(patch.bank).\(patch.num)\t\t\(patch.name)")
                        Label("\(patch.bank).\(patch.num)\t\t\(patch.name)", systemImage: "music.note.list")
                }
                .onMove (perform: move)
            }
        }
        .navigationTitle("Ampero Patch Organizer")
        .refreshable {
        }
        
        Text("Selected \(multiSelection.count) lines")
        
        HStack {
            Button(action: actionReload) {
                Label("Reload", systemImage: "arrow.counterclockwise")
            }
            Button(action: actionDummy) {
                Label("Up", systemImage: "arrow.up")
            }
            Button(action: actionDummy) {
                Label("Down", systemImage: "arrow.down")
            }
            Button(action: actionOpen) {
                Label("Open...", systemImage: "doc")
            }
            Button(action: actionExport) {
                Label("Export...", systemImage: "arrow.down.doc")
            }
        }
        .buttonStyle(.bordered)
        
        Text("File: \(patches.fileName)")
    }
    
    func move(from source: IndexSet, to destination: Int) {
        patches.patches.move(fromOffsets: source, toOffset: destination)
    }
    func actionOpen() {
        let panel = NSOpenPanel()
        panel.title = "Open patches list"
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        if panel.runModal() == .OK {
            patches.readFromFile(fileName: panel.url?.path ?? "<none>")
        }
    }
    func actionExport() {
        let panel = NSSavePanel() //NSOpenPanel()
        panel.title = "Export patches list"
        //panel.allowsMultipleSelection = false
        //panel.canChooseDirectories = false
        if panel.runModal() == .OK {
            patches.writeToFile(fileName: panel.url?.path ?? "<none>")
        }
    }
    
    func actionReload() {
        patches.reReadFromFile()
    }
    func actionDummy() {
        
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
