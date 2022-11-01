//
//  VersionControler.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 01/11/2022.
//

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
            newVersion = dv != actualVersion
        }
    }
    
    func getData() async throws {
        let url1 = URL(string: "https://api.github.com/repos/ThibaultDucray/PatchOrganizer/releases/latest")!
        let vtag = try await URLSession.shared.getGitHubRelease(from: url1)
        distantVersion = vtag.tag_name
    }
}
