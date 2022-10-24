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
}

class UIPatches: ObservableObject {
    @Published var patches = dummypatches
    @Published var fileName = "<nofilename>"
    var newFileName = "<nofilename>"
    @Published var openError = false
    @Published var saveError = false

    func readFromFile(fileName: String) {
        self.fileName = fileName
        openError = false
        let patchHandler = PatchHandler(fileName: fileName)
        if (patchHandler.validFile) {
            // todo? make this func async
            patches = []
            for i in (0 ... NBPATCHES - 1) {
                let p = UIPatch(id: Int(i), elem: patchHandler.getElem(i: Int(i)))
                patches.append(p)
            }
            patches.sort(by: {(a:UIPatch, b:UIPatch) -> Bool in return (a.bank * 3 + a.num) < (b.bank * 3 + b.num) })
        } else {
            openError = true
        }
    }
    
    func reReadFromFile() {
        readFromFile(fileName: fileName)
    }
    
    func writeToFile(fileName: String) {
        saveError = false
        self.newFileName = fileName
        let patchHandler = PatchHandler(fileName: self.fileName)
        
        for i in (0 ... NBPATCHES - 1) {
            let id = patches[Int(i)].id
            let bank = patches[Int(i)].bank
            let num = patches[Int(i)].num
            let name = patches[Int(i)].name
            patchHandler.setElem(actualpos: id, bank: bank, num: num, name: name, newpos: Int(i))
        }
        let err = patchHandler.writePatchlist(fileName: self.newFileName)
        saveError = (err <= 0)
    }
}

struct ContentView: View {
    @State private var multiSelection = Set<Int>()
    @ObservedObject var uiPatches = UIPatches()
    
    var body: some View {
        
        VStack {
            Text("Patches list")
            List (selection: $multiSelection) {
                
                ForEach(uiPatches.patches, id: \.id) { patch in
                    //Text("\(patch.bank).\(patch.num)\t\t\(patch.name)")
                        Label("\(patch.bank).\(patch.num)\t\t\(patch.name)", systemImage: "music.note.list")
                }
                .onMove (perform: move)
            }
        }
        .alert("Could not export to \(uiPatches.newFileName)", isPresented: $uiPatches.saveError, actions: {
            Button("OK", role: .cancel, action: {})
            })
        .alert("Could not open \(uiPatches.fileName)", isPresented: $uiPatches.openError, actions: {
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
            Button(action: actionExport) {
                Label("Export...", systemImage: "arrow.down.doc")
            }
        }
        .buttonStyle(.bordered)
        
        Text("File: \(uiPatches.fileName)")
    }
    
    func move(from source: IndexSet, to destination: Int) {
        var s = ""
        source.forEach { elem in
            s = s + "\(elem) "
        }
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
    func actionExport() {
        let panel = NSSavePanel() //NSOpenPanel()
        panel.title = "Export patches list"
        //panel.allowsMultipleSelection = false
        //panel.canChooseDirectories = false
        if panel.runModal() == .OK {
            uiPatches.writeToFile(fileName: panel.url?.path ?? "<none>")
        }
    }
    
    func actionReload() {
        uiPatches.reReadFromFile()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
