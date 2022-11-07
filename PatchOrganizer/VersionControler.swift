//  VersionControler.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 01/11/2022.
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

import Foundation

struct GitHubRelease: Codable {
    var tag_name: String
}

extension URLSession {
    func getGitHubRelease(
        from url: URL,
        keyDecodingStrategy: JSONDecoder.KeyDecodingStrategy = .useDefaultKeys,
        dataDecodingStrategy: JSONDecoder.DataDecodingStrategy = .deferredToData,
        dateDecodingStrategy: JSONDecoder.DateDecodingStrategy = .deferredToDate
    ) async throws  -> GitHubRelease {
        let (data, _) = try await data(from: url)
        let decoder = JSONDecoder()
        decoder.keyDecodingStrategy = keyDecodingStrategy
        decoder.dataDecodingStrategy = dataDecodingStrategy
        decoder.dateDecodingStrategy = dateDecodingStrategy
        let decoded = try decoder.decode(GitHubRelease.self, from: data)
        return decoded
    }
}

class VersionControler: ObservableObject {
    let actualVersion: String
    var distantVersion: String?
    @Published var newVersion = false
    
    init() {
        //First get the nsObject by defining as an optional anyObject
        //let nsObject: AnyObject? = Bundle.mainBundle().infoDictionary["CFBundleShortVersionString"]
        actualVersion = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as! String
        //Then just cast the object as a String, but be careful, you may want to double check for nil
        Task {
            try await getData()
        }
    }
    
    func control() {
        if let dv = distantVersion {
            newVersion = greater(low: actualVersion, hi: dv)
        }
    }
    
    func getData() async throws {
        let url1 = URL(string: "https://api.github.com/repos/ThibaultDucray/PatchOrganizer/releases/latest")!
        let vtag = try await URLSession.shared.getGitHubRelease(from: url1)
        distantVersion = vtag.tag_name
    }
    
    func versionToInt(s: String) -> [UInt16] {
        let s1 = s.uppercased().replacingOccurrences(of: "V", with: "")
        var s1s = s1.split(separator: ".")
        while s1s.count < 3 {
            s1s.append("0")
        }
        var val: [UInt16] = []
        for i in 0..<s1s.count {
            let h = UInt16(s1s[i]) ?? 0
            val.append(h)
        }
        return val
    }
    
    func greater(low: String, hi: String) -> Bool {
        let lowv = versionToInt(s: low)
        let hiv = versionToInt(s: hi)
        for i in 0..<lowv.count {
            if hiv[i] != lowv[i] {
                if hiv[i] > lowv[i] {
                    return true
                }
                else {
                    return false
                }
            }
        }
        return false
    }
}
