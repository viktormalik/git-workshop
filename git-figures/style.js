const options = {
    template: GitgraphJS.templateExtend("metro", {
        colors: ["#008fb5", "#f1c109", "#979797"],
        commit: {
            message: {
                displayAuthor: false,
                displayEmail: false,
                displayBranch: true,
                displayHash: false,
                font: "normal 14pt Overpass"
            }
        },
        branch: {
            label: {
                font: "normal 14pt Overpass"
            }
        }
    }),
};
