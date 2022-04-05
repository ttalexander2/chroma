import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';
import * as yaml from 'yaml';

export class CodeCompletionItemProvider implements vscode.CompletionItemProvider {

    private completions: vscode.CompletionItem[] = [];

    public provideCompletionItems(
        document: vscode.TextDocument, position: vscode.Position, token: vscode.CancellationToken): vscode.CompletionItem[] | Thenable<vscode.CompletionItem[]> {
        this.completions = [];

        let workspaces = vscode.workspace.workspaceFolders
        if (workspaces !== undefined) {
            for (let workspace of workspaces) {
                let asset_path = path.join(workspace.uri.fsPath, "Assets.yaml");
                let data = fs.readFileSync(asset_path, 'utf8');
                if (data !== undefined){
                    let yaml_result = yaml.parse(data);
                    for (const guid in yaml_result) {
                        const path = yaml_result[guid]['Path']
                        const commitCharacterCompletion = new vscode.CompletionItem(path);
                        commitCharacterCompletion.documentation = new vscode.MarkdownString("\"" + guid + "\"");
                        commitCharacterCompletion.insertText = "\"" + guid + "\"";
                        this.completions.push(commitCharacterCompletion);
                    }
                }
            }  
        }
        return this.completions;
    }
        
}