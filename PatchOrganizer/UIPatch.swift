//  UIPatch.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 03/11/2022.
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

struct UIPatch: Identifiable {
    let id: Int
    var bank: Int
    var num: Int
    var userIR: Bool
    var name: String
    
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
    @Published var replaceError = false
    var errcode = 0
    var patchHandler: PatchHandler?
    
    func actionOpen() {
        let panel = NSOpenPanel()
        panel.title = "Open patches list"
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        //panel.allowedContentTypes = [UTType(filenameExtention: "prst")]
        if panel.runModal() == .OK {
            self.readFromFile(fileName: panel.url?.path ?? "<none>")
        }
    }
    
    func actionReload() {
        if actualFileName == nil {
            emptyError = true
            return
        }
        reReadFromFile()
    }
    
    func actionSaveAs() {
        if actualFileName == nil {
            emptyError = true
            return
        }
        let panel = NSSavePanel() //NSOpenPanel()
        panel.title = "Save patches list"
        panel.nameFieldStringValue = ((actualFileName ?? "patches.prst") as NSString).lastPathComponent
        if panel.runModal() == .OK {
            writeToFile(newFileName: panel.url?.path ?? "<none>")
        }
    }
    
    func readFromFile(fileName: String) {
        self.actualFileName = fileName
        patches = []
        openError = false
        patchHandler = PatchHandler(fileName: fileName)
        if let vf = patchHandler?.validFile {
            if (vf) {
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
    
    func actionMove(from source: IndexSet, to destination: Int) {
        patches.move(fromOffsets: source, toOffset: destination)
        for pos in 0..<patches.count {
            let uip = patches[pos]
            if let p = patchHandler {
                p.setElem(actualpos: uip.id, name: uip.name, newpos: pos)
            }
        }
    }
    
    func actionReplace(index: Int) {
            let panel = NSOpenPanel()
            panel.title = "Open patches list"
            panel.allowsMultipleSelection = false
            panel.canChooseDirectories = false
            //panel.allowedContentTypes = [UTType(filenameExtention: "prst")]
            if panel.runModal() == .OK {
                if let fileName = panel.url?.path {
                    replacePatch(sourceFileName: fileName, destIndex: index)
                }
            }
        }
    
    func findPatchFromId(id: Int) -> Int? {
        for i in 0..<patches.count {
            if patches[i].id == id {
                return i
            }
        }
        return nil
    }
    
    func replacePatch(sourceFileName: String, destIndex: Int) {
        let otherPatches = UIPatches()
        otherPatches.readFromFile(fileName: sourceFileName)
        if otherPatches.patches.count == 1 {
            if let oph = otherPatches.patchHandler {
                patchHandler?.replacePatch(source: oph, from: 0, to: destIndex)
                if let elem = patchHandler?.getElem(i: destIndex) {
                    if let i = findPatchFromId(id: destIndex) {
                        patches[i] = UIPatch(id: destIndex, elem: elem)
                        return
                    }
                }
            }
        }
        replaceError = true
    }
    
    func exportOnePatch(index: Int) {
        var patchname: String = "patch"
        if patchHandler != nil {
            (bank: _, num: _, userIR: _, name: patchname) = patchHandler!.getElem(i: index)
        }
        let panel = NSSavePanel()
        panel.title = "Export one patch"
        panel.nameFieldStringValue = "\(patchname).prst"
        if panel.runModal() == .OK {
            let fileName = panel.url?.path ?? "<none>"
            if let err = patchHandler?.exportOnePatch(fileName: fileName, index: index, invertTailBit: invertTailBit) {
                saveError = (err <= 0);
            } else {
                saveError = true
            }
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
                //let bank = patches[Int(i)].bank
                //let num = patches[Int(i)].num
                let name = patches[Int(i)].name
                patchHandler?.setElem(actualpos: id, name: name, newpos: Int(i))
            }
        }
        if let err = patchHandler?.writePatchlist(fileName: newFileName, invertTailBit: invertTailBit) {
            self.errcode = err
            saveError = (err <= 0)
        }
    }
}
