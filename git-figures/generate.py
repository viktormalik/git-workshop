#!/usr/bin/env python3
# Usage: 
#
#   ./generate <filename> [list of branches] <start commit>
#
# where:
#   filename         is the name of HTML and PDF files (without extension) to 
#                    create
#   list of branches gives a list of branches to include in the graph
#                    they must be given in the reverse order to the order in
#                    which they were created
#   start commit     is hash of the commit at which the graph starts 
#                    or "all" if all the commits should be included
#
# Example:
#   ./generate test master all
# to generate graph from the current master branch into test.pdf (and test.html)
#
# Requirements:
#   whhtmltopdf, pdfcrop
import sys
import os
import subprocess
from git import Repo

repo_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
repo = Repo(repo_path)

html = """<!DOCTYPE html>
<html>
<head>
    <!-- Load the JS file -->
    <script src="https://cdn.jsdelivr.net/npm/@gitgraph/js"></script>
    <script src="style.js"></script>
</head>
<body>
    <!-- DOM element in which we'll mount our graph -->
    <div id="graph-container"></div>

    <script>
        // Get the graph container HTML element.
        const graphContainer = document.getElementById("graph-container");

        // Instantiate the graph.
        const gitgraph = GitgraphJS.createGitgraph(graphContainer, options);
"""


def has_unexplored_head(repo, commit, heads_to_explore):
    for head in heads_to_explore:
        if head.commit == commit:
            return True
    return False

branches_html = ""
branch_prev = ""

heads_to_explore = [repo.heads[h] for h in sys.argv[2:-1]]
while len(heads_to_explore) > 0:
    head = heads_to_explore.pop(0)
    commit = head.commit

    var_name = head.name.replace("-", "_")
    branch_label = head.name
    # GitGraph.js does not show labels for branches with no commits, therefore
    # it is neccessary to include them in the label of the following branch
    if branch_prev:
        branch_label += ", " + branch_prev

    branch_html = ""

    while not has_unexplored_head(repo, commit, heads_to_explore):
        commit_html = """
        {}.commit({{
            subject: "{}",
            author: "{}",
        }});
        """.format(var_name, commit.message.splitlines()[0], commit.author.name)
        branch_html = commit_html + branch_html

        if len(commit.parents) == 0 or (sys.argv[-1] != "all" and
                                        commit == repo.commit(sys.argv[-1])):
            break

        commit = commit.parents[0]

    if not branch_html:
        branch_prev = branch_label
    else:
        branch_prev = ""

    branches_html = """
        const {} = gitgraph.branch(\"{}\")
    """.format(var_name, branch_label) + branch_html + branches_html

html += branches_html
html +="""
    </script>
</body>
</html>
"""

filename = sys.argv[1]
html_name = "{}.html".format(filename)
pdf_name = "{}.pdf".format(filename)

with open(html_name, "w") as html_file:
    html_file.write(html)

subprocess.check_call(["wkhtmltopdf", html_name, pdf_name])
subprocess.check_call(["pdfcrop", pdf_name, pdf_name])

