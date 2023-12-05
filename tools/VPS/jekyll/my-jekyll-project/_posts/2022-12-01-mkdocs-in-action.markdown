---
layout: post
title:  "MkDocs in Action"
date:   2022-12-01 20:00:00 +0800
categories: Tools
---

* Do not remove this line (it will not be displayed)
{:toc}

> Project documentation with Markdown.

MkDocs is a **fast**, **simple** and **downright gorgeous** static site generator that's geared towards building project documentation. Documentation source files are written in `Markdown`, and configured with a single `YAML` configuration file. Start by reading the [introductory tutorial](https://www.mkdocs.org/getting-started/), then check the [User Guide](https://www.mkdocs.org/user-guide/) for more information.

Features:

* Great themes available
* Easy to customize
* Preview your site as you work
* Host anywhere


For full documentation visit [mkdocs.org](https://www.mkdocs.org/).


# Commands

`mkdocs new [dir-name]` - Create a new project.
`mkdocs serve` - Start the live-reloading docs server.
`mkdocs build` - Build the documentation site.
`mkdocs -h` - Print help message and exit.

# Project layout

```
mkdocs.yml    # The configuration file.
docs/
    index.md  # The documentation homepage.
    ...       # Other markdown pages, images and other files.
```

# [Getting Started with MkDocs](https://www.mkdocs.org/getting-started/)

## Installation

```
pip install mkdocs
```

## Creating a new project

```
mkdocs new my-project
cd my-project
```

```
~/tools/mkdocs$mkdocs new my-project
INFO     -  Creating project directory: my-project
INFO     -  Writing config file: my-project/mkdocs.yml
INFO     -  Writing initial docs: my-project/docs/index.md
~/tools/mkdocs$ls
my-project
~/tools/mkdocs$tree my-project/
my-project/
├── docs
│   └── index.md
└── mkdocs.yml

1 directory, 2 files
```

There's a single configuration file named `mkdocs.yml`, and a folder named `docs` that will contain your documentation source files (`docs` is the default value for the [docs_dir](https://www.mkdocs.org/user-guide/configuration/#docs_dir) configuration setting). Right now the `docs` folder just contains a single documentation page, named [index.md](https://www.mkdocs.org/user-guide/configuration/#docs_dir).

`MkDocs` comes with a built-in dev-server that lets you preview your documentation as you work on it. Make sure you're in the same directory as the `mkdocs.yml` configuration file, and then start the server by running the `mkdocs serve` command:

```
$mkdocs serve -a 9.135.18.186:8080
INFO     -  Building documentation...
INFO     -  Cleaning site directory
INFO     -  Documentation built in 0.11 seconds
INFO     -  [20:21:41] Watching paths for changes: 'docs', 'mkdocs.yml'
INFO     -  [20:21:41] Serving on http://9.135.18.186:8080/
INFO     -  [20:21:47] Browser connected: http://9.135.18.186:8080/
INFO     -  [20:22:41] Browser connected: http://9.135.18.186:8080/
INFO     -  [20:25:15] Browser connected: http://9.135.18.186:8080/
INFO     -  [20:30:35] Detected file changes
INFO     -  Building documentation...
INFO     -  [20:30:35] Reloading browsers
INFO     -  [20:30:35] Browser connected: http://9.135.18.186:8080/

```

Open up http://9.135.18.186:8000/ in your browser, and you'll see the default home page being displayed.

![mkdocs_home](/assets/images/202212/mkdocs_home.png)

The dev-server also supports **auto-reloading**, and will rebuild your documentation whenever anything in the configuration file, documentation directory, or theme directory changes.

Open the `docs/index.md` document in your text editor of choice, change the initial heading to `MkLorum`, and save your changes. Your browser will auto-reload and you should see your updated documentation immediately.

Now try editing the configuration file: `mkdocs.yml`. Change the [site_name](https://www.mkdocs.org/user-guide/configuration/#site_name) setting to MkLorum and save the file.

``` yaml
site_name: MkLorum
site_url: https://example.com/
```

Your browser should immediately reload, and you'll see your new site name take effect.

> **Note**:
>
> The [site_name](https://www.mkdocs.org/user-guide/configuration/#site_name) and [site_url](https://www.mkdocs.org/user-guide/configuration/#site_url) configuration options are the only two required options in your configuration file. When you create a new project, the `site_url` option is assigned the placeholder value: `https://example.com`. If the final location is known, you can change the setting now to point to it. Or you may choose to leave it alone for now. Just be sure to edit it before you deploy your site to a production server.


## Adding pages

Now add a second page to your documentation:

```
curl 'https://jaspervdj.be/lorem-markdownum/markdown.txt' > docs/about.md
```

As our documentation site will **include some navigation headers**, you may want to edit the configuration file and add some information about the order, title, and nesting of each page in the navigation header by adding a nav setting:

``` yaml
site_name: MkLorum
site_url: https://example.com/
nav:
    - Home: index.md
    - About: about.md
```

Save your changes and you'll now see a navigation bar with `Home` and `About` items on the left as well as `Search`, `Previous`, and `Next` items on the right.

![mkdocs_home2](/assets/images/202212/mkdocs_home2.png)

Try the menu items and navigate back and forth between pages. Then click on **Search**. A search dialog will appear, **allowing you to search for any text on any page**. Notice that the search results include every occurrence of the search term on the site and links directly to the section of the page in which the search term appears. You get all of that with no effort or configuration on your part!

![mkdocs_home3](/assets/images/202212/mkdocs_home3.png)


## Theming our documentation

Now change the configuration file to alter how the documentation is displayed by changing the theme. Edit the `mkdocs.yml` file and add a [theme](https://www.mkdocs.org/user-guide/configuration/#theme) setting:

``` yaml
site_name: MkLorum
site_url: https://example.com/
nav:
    - Home: index.md
    - About: about.md
theme: readthedocs
```

Save your changes, and you'll see the ReadTheDocs theme being used.

![mkdocs_home4](/assets/images/202212/mkdocs_home4.png)


## Changing the Favicon Icon

By default, MkDocs uses the [MkDocs favicon](https://www.mkdocs.org/img/favicon.ico) icon. To use a different icon, create an `img` subdirectory in the `docs` directory and copy your custom `favicon.ico` file to that directory. MkDocs will automatically detect and use that file as your favicon icon.

## Building the site

That's looking good. You're ready to deploy the first pass of your `MkLorum` documentation. First build the documentation:

```
mkdocs build
```

This will create a new directory, named `site`. Take a look inside the directory:

```
$ ls site
about  fonts  index.html  license  search.html
css    img    js          mkdocs   sitemap.xml
```

```
~/tools/mkdocs/my-project$mkdocs build
INFO     -  Cleaning site directory
INFO     -  Building documentation to directory: /data/home/gerryyang/tools/mkdocs/my-project/site
INFO     -  Documentation built in 0.10 seconds
~/tools/mkdocs/my-project$ls
docs  mkdocs.yml  site
~/tools/mkdocs/my-project$cd site/
~/tools/mkdocs/my-project/site$ls
404.html  about  css  img  index.html  js  search  search.html  sitemap.xml  sitemap.xml.gz
```

Notice that your source documentation has been output as two HTML files named `index.html` and `about/index.html`. You also have various other media that's been copied into the site directory as part of the documentation theme. You even have a `sitemap.xml` file and `mkdocs/search_index.json`.

If you're using source code control such as `git` you probably don't want to check your documentation builds into the repository. Add a line containing `site/` to your `.gitignore` file.

```
echo "site/" >> .gitignore
```

If you're using another source code control tool you'll want to check its documentation on how to ignore specific directories.

## Other Commands and Options

There are various other commands and options available. For a complete list of commands, use the `--help` flag:

```
mkdocs --help
```

To view a list of options available on a given command, use the `--help` flag with that command. For example, to get a list of all options available for the `build` command run the following:

```
mkdocs build --help
```

## Deploying

The documentation site that you just built only uses static files so you'll be able to host it from pretty much anywhere. Simply upload the contents of the entire `site` directory to wherever you're hosting your website from and you're done. For specific instructions on a number of common hosts, see the [Deploying your Docs](https://www.mkdocs.org/user-guide/deploying-your-docs/) page.

## Getting help

See the [User Guide](https://www.mkdocs.org/user-guide/) for more complete documentation of all of MkDocs' features.

To get help with MkDocs, please use the [GitHub discussions](https://github.com/mkdocs/mkdocs/discussions) or [GitHub issues](https://github.com/mkdocs/mkdocs/issues).



# Other Tools

* https://obsidian.md/

