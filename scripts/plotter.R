library(stats)
library(gsubfn)
library(ggplot2)
library(dplyr)
library(extrafont)
library(patchwork)
library(grid)
library(gtable)

## This script plots all graphs used in the associated paper
## using the data files created with 'aggregate.sh'





## -----------------------------------------------------------
## How to have 'Linux Libertine' font working for the script
##
## 1. Download the font in TTF format (e.g., from here: https://fontmeme.com/fonts/linux-libertine-font/)
## 2. Place the .ttf files in a folder './libertine/'
## 3. Open R, load the 'remotes' library and run the following command to fix issues with Rttf2pt1 (https://github.com/wch/extrafont/issues/32):
##       remotes::install_version("Rttf2pt1", version = "1.3.8")
## 4. Then, load the 'extrafont' library and run the command:
##       font_import(path = "./libertine/")
## 5. Check whether 'Linux Libertine' fonts appear in fonts()
## 6. Because of how 'Linux Libertine' fonts are defined, we need to rename the font table from R (https://stackoverflow.com/questions/38596300/r-using-libertine-font-in-eps-error-message). For that, find where the table is saved in your system with the command:
##       system.file("fontmap", "fonttable.csv", package="extrafontdb")
##    Open file 'fonttable.csv' and copy the values from column 'FullName' to 'FamilyName'. Afterwards, run the command (in R):
##       load_fonts()
##    to make sure R rebuilds the table.
##  7. Load the fonts to 'pdf' device via command:
##       loadfonts(device = "pdf", quiet = TRUE)
##  8. You are finally ready to run this script!
## -----------------------------------------------------------

## a color-blind-friendly palette
cbPalette <- c("#88CCEE", "#CC6677", "#DDCC77", "#117733", "#332288", "#AA4499", "#44AA99", "#999933", "#882255", "#661100", "#6699CC", "#888888")

ALGS <- c("ULT", "KAB", "KAJ", "CP", "SCP", "ILP", "CRA", "RULT", "BFDC")

setup_shapes <- function(data) {
    shapes <- c()
    lines <- c()
    colors <- c()
    cur <- ""

    for(i in 1:nrow(data)){
        if(cur != data[i,]$Method){
            cur <- data[i,]$Method
            if(cur == "BFDC"){
                shapes <- append(shapes, 16)
                lines <- append(lines, 1)
                colors <- append(colors, cbPalette[6])
            }else if(cur == "RULT"){
                shapes <- append(shapes, 15)
                lines <- append(lines, 2)
                colors <- append(colors, cbPalette[5])
            }else if(cur == "CRA"){
                shapes <- append(shapes, 17)
                lines <- append(lines, 4)
                colors <- append(colors, cbPalette[4])
            }else if(cur == "ILP"){
                shapes <- append(shapes, 4)
                lines <- append(lines, 1)
                colors <- append(colors, cbPalette[2])
            }else if(cur == "CP"){
                shapes <- append(shapes, 8)
                lines <- append(lines, 3)
                colors <- append(colors, cbPalette[11])
            }else if(cur == "SCP"){
                shapes <- append(shapes, 18)
                lines <- append(lines, 6)
                colors <- append(colors, cbPalette[8])
            }else if(cur == "KAB"){
                shapes <- append(shapes, 6)
                lines <- append(lines, 2)
                colors <- append(colors, cbPalette[10])
            }else if(cur == "KAJ"){
                shapes <- append(shapes, 13)
                lines <- append(lines, 6)
                colors <- append(colors, cbPalette[7])
            }else if(cur == "ULT"){
                shapes <- append(shapes, 11)
                lines <- append(lines, 4)
                colors <- append(colors, cbPalette[12])
            }
        }
    }

    return( list(shapes,lines,colors) )
}

scientific_10 <- function(x) {
    e <- parse(text=gsub("e", " %*% 10^", scales::scientific_format()(x)))
    if(!is.na(x[1])){
        if(x[1] == 0) e[1] <- parse(text="0")
    }
    return(e)
}

firstup <- function(x) {
    #print(x)
    substr(x, 1, 1) <- toupper(substr(x, 1, 1))
    x
}

load_data <- function(base) {

    if(base == "Negcycle"){
        name1 <- sprintf("../data/time/%s/sum_nc02.txt", toupper(base))
        name2 <- sprintf("../data/time/%s/sum_nc03.txt", toupper(base))
        name3 <- sprintf("../data/time/%s/sum_nc04.txt", toupper(base))
        name4 <- sprintf("../data/time/%s/sum_nc05.txt", toupper(base))

        d1 <- read.table(name1, header=T, sep=";")
        d2 <- read.table(name2, header=T, sep=";")
        d3 <- read.table(name3, header=T, sep=";")
        d4 <- read.table(name4, header=T, sep=";")

        d1$Group <- base
        d2$Group <- base
        d3$Group <- base
        d4$Group <- base

        d1$Subgroup <- "Nc02"
        d2$Subgroup <- "Nc03"
        d3$Subgroup <- "Nc04"
        d4$Subgroup <- "Nc05"

        alld <- rbind(d1, d2)
        alld <- rbind(alld, d3)
        alld <- rbind(alld, d4)

        return(alld)
    }else if(base == "Vrp"){
        name1 <- sprintf("../data/time/%s/sum_feas_vrp.txt", toupper(base))
        name2 <- sprintf("../data/time/%s/sum_infeas_vrp.txt", toupper(base))
        d1 <- read.table(name1, header=T, sep=";")
        d2 <- read.table(name2, header=T, sep=";")

        d1$Group <- base
        d2$Group <- base

        d1$Subgroup <- "Feasible"
        d2$Subgroup <- "Infeasible"

        alld <- rbind(d1, d2)
        return(alld)
    }else if(base == "Vl"){
        name1 <- sprintf("../data/time/%s/sum_vl.txt", toupper(base))
        d1 <- read.table(name1, header=T, sep=";")
        d1$Group <- base
        return(d1)
    }else if(base == "Chain"){
        name1 <- sprintf("../data/time/%s/sum_chain.txt", toupper(base))
        d1 <- read.table(name1, header=T, sep=";")
        d1$Group <- base
        return(d1)
    }else{
        name1 <- sprintf("../data/time/%s/sum_nodes.txt", toupper(base))
        name2 <- sprintf("../data/time/%s/sum_density.txt", toupper(base))
        name3 <- sprintf("../data/time/%s/sum_num-disjunctions.txt", toupper(base))
        name4 <- sprintf("../data/time/%s/sum_var-disjunctions.txt", toupper(base))

        d1 <- read.table(name1, header=T, sep=";")
        d2 <- read.table(name2, header=T, sep=";")
        d3 <- read.table(name3, header=T, sep=";")
        d4 <- read.table(name4, header=T, sep=";")

        d1$Group <- base
        d2$Group <- base
        d3$Group <- base
        d4$Group <- base

        d1$Subgroup <- "Nodes"
        d2$Subgroup <- "Density"
        d3$Subgroup <- "NumDisj"
        d4$Subgroup <- "VarDisj"

        alld <- rbind(d1, d2)
        alld <- rbind(alld, d3)
        alld <- rbind(alld, d4)

        return(alld)
    }
}


create_vrp_graphs <- function(){
    gf1 <- "../data/time/graphs/graph_all_vrp.pdf"

    data <- load_data("Vrp")

    data <- data %>%
        group_by(Size, Method, Group, Subgroup) %>%
        summarize(tavg = mean(Time),
                  std = sd(Time),
                  tmin = min(Time),
                  tmax = max(Time),
                  tsum = sum(Time),
                  count = length(Time))


    ##head(data)
    data$Method <- as.factor(data$Method)
    data$Method <- factor(data$Method, levels=ALGS)
    data <- data[order(data$Method),]

    shapes <- c()
    lines <- c()
    colors <- c()
    list[shapes, lines, colors] <- setup_shapes(data)
    data$Subgroup <- factor(data$Subgroup, levels = c("Feasible", "Infeasible"))

    plot <- ggplot(data, aes(x=Size, y=tavg, group=Method, color=Method, shape=Method, linetype=Method)) +
        labs(y = "Time (\u00B5s)", x = expression(paste("Number of time-points (|T|)"))) +
        facet_grid(cols = vars(data$Subgroup), scales="free") +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine")) +
        guides(colour = guide_legend(override.aes = list(size=3))) +
        scale_shape_manual(values=shapes) +
        scale_linetype_manual(values=lines)+
        scale_color_manual(values=colors) +
        scale_y_continuous(trans='log10', label=scientific_10) +
        #scale_x_continuous(trans='log10') +
        geom_point(size=1, alpha=0.4)
        #stat_smooth()

    ggsave(filename=gf1, width=10, height=4, plot=plot)
    embed_fonts(gf1)

}

create_nc_graphs <- function(){
    gf1 <- "../data/time/graphs/graph_all_nc.pdf"

    data <- load_data("Negcycle")

    data <- data %>%
        group_by(Size, Method, Group, Subgroup) %>%
        summarize(tavg = mean(Time),
                  std = sd(Time),
                  tmin = min(Time),
                  tmax = max(Time),
                  tsum = sum(Time),
                  count = length(Time))


    ##head(data)
    data$Method <- as.factor(data$Method)
    data$Method <- factor(data$Method, levels=ALGS)
    data <- data[order(data$Method),]

    shapes <- c()
    lines <- c()
    colors <- c()
    list[shapes, lines, colors] <- setup_shapes(data)
    data$Subgroup <- factor(data$Subgroup, levels = c("Nc02", "Nc03", "Nc04", "Nc05"))

    plot <- ggplot(data, aes(x=Size, y=tavg, group=Method, color=Method, shape=Method, linetype=Method)) +
        labs(y = "Time (\u00B5s)", x = expression(paste("Number of time-points (|T|)"))) +
        facet_grid(cols = vars(data$Subgroup), scales="free") +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine")) +
        scale_shape_manual(values=shapes) +
        scale_linetype_manual(values=lines)+
        scale_color_manual(values=colors) +
        scale_y_continuous(trans='log10', label=scientific_10) +
        scale_x_continuous(trans='log10') +
        geom_point(size=2, alpha=0.6) +
        geom_line()

    ggsave(filename=gf1, width=10, height=4, plot=plot)
    embed_fonts(gf1)
}

create_sp_graphs <- function(){
    gfnodes <- "../data/time/graphs/graph_sp_nodes.pdf"
    gfdensity <- "../data/time/graphs/graph_sp_density.pdf"
    gfnumd <- "../data/time/graphs/graph_sp_numd.pdf"
    gfvard <- "../data/time/graphs/graph_sp_vard.pdf"

    data1 <- load_data("Rand")
    data2 <- load_data("Grid")
    data3 <- load_data("Seq")
    data4 <- load_data("Late")
    data <- rbind(data1, data2)
    data <- rbind(data, data3)
    data <- rbind(data, data4)

    ##head(data)

    data <- data %>%
        group_by(Size, Method, Group, Subgroup) %>%
        summarize(tavg = mean(Time),
                  std = sd(Time),
                  tmin = min(Time),
                  tmax = max(Time),
                  tsum = sum(Time),
                  count = length(Time))


    ##head(data)
    data$Method <- as.factor(data$Method)
    data$Method <- factor(data$Method, levels=ALGS)
    data <- data[order(data$Method),]

    dN <- data[data$Subgroup == "Nodes",]
    shapes <- c()
    lines <- c()
    colors <- c()
    list[shapes, lines, colors] <- setup_shapes(dN)
    dN$Group <- factor(dN$Group, levels = c("Rand", "Grid", "Seq", "Late"))

    plot1 <- ggplot(dN, aes(x=Size, y=tavg, group=Method, color=Method, shape=Method, linetype=Method)) +
        labs(y = "Time (\u00B5s)", x = "Number of time-points (|T|)") +
        facet_grid(cols = vars(dN$Group), scales="free") +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine")) +
        scale_shape_manual(values=shapes) +
        scale_linetype_manual(values=lines)+
        scale_color_manual(values=colors) +
        scale_y_continuous(trans='log10', label=scientific_10) +
        scale_x_continuous(trans='log10') +
        geom_point(size=2, alpha=0.6) +
        geom_line()


    dD <- data[data$Subgroup == "Density",]
    shapes <- c()
    lines <- c()
    colors <- c()
    list[shapes, lines, colors] <- setup_shapes(dD)
    dD$Group <- factor(dD$Group, levels = c("Rand", "Grid", "Seq", "Late"))

    plot2 <- ggplot(dD, aes(x=Size, y=tavg, group=Method, color=Method, shape=Method, linetype=Method)) +
        labs(y = "Time (\u00B5s)", x = expression("Density of C"[1]*" constraints")) +
        facet_grid(cols = vars(dD$Group), scales="free") +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine")) +
        scale_shape_manual(values=shapes) +
        scale_linetype_manual(values=lines)+
        scale_color_manual(values=colors) +
        scale_y_continuous(trans='log10', label=scientific_10) +
        scale_x_continuous(breaks=c(0.25,0.50, 0.75)) +
        geom_point(size=2, alpha=0.6) +
        geom_line()


    dND <- data[data$Subgroup == "NumDisj",]
    shapes <- c()
    lines <- c()
    colors <- c()
    list[shapes, lines, colors] <- setup_shapes(dND)
    dND$Group <- factor(dND$Group, levels = c("Rand", "Grid", "Seq", "Late"))

    plot3 <- ggplot(dND, aes(x=Size, y=tavg, group=Method, color=Method, shape=Method, linetype=Method)) +
        labs(y = "Time (\u00B5s)", x = expression("Num. domains K per time-point in T"["D"])) +
        facet_grid(cols = vars(dND$Group), scales="free") +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine")) +
        scale_shape_manual(values=shapes) +
        scale_linetype_manual(values=lines)+
        scale_color_manual(values=colors) +
        scale_y_continuous(trans='log10', label=scientific_10) +
        geom_point(size=2, alpha=0.6) +
        geom_line()


    dV <- data[data$Subgroup == "VarDisj",]
    shapes <- c()
    lines <- c()
    colors <- c()
    list[shapes, lines, colors] <- setup_shapes(dND)
    dV$Group <- factor(dV$Group, levels = c("Rand", "Grid", "Seq", "Late"))

    plot4 <- ggplot(dV, aes(x=Size, y=tavg, group=Method, color=Method, shape=Method, linetype=Method)) +
        labs(y = "Time (\u00B5s)", x = expression("Perc. of time-points in T"["D"])) +
        facet_grid(cols = vars(dV$Group), scales="free") +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine")) +
        scale_shape_manual(values=shapes) +
        scale_linetype_manual(values=lines)+
        scale_color_manual(values=colors) +
        scale_y_continuous(trans='log10', label=scientific_10) +
        geom_point(size=2, alpha=0.6) +
        geom_line()


    ##finally, save the plot
    ggsave(filename=gfnodes, width=10, height=3.5, plot=plot1)
    ggsave(filename=gfdensity, width=10, height=3.5, plot=plot2)
    ggsave(filename=gfnumd, width=10, height=3.5, plot=plot3)
    ggsave(filename=gfvard, width=10, height=3.5, plot=plot4)
    embed_fonts(gfnodes)
    embed_fonts(gfdensity)
    embed_fonts(gfnumd)
    embed_fonts(gfvard)
}

create_vl_graphs <- function(){
    gf1 <- "../data/time/graphs/graph_all_vl.pdf"

    data <- load_data("Vl")

    data <- data %>%
        group_by(Size, Method, Group) %>%
        summarize(tavg = mean(Time),
                  std = sd(Time),
                  tmin = min(Time),
                  tmax = max(Time),
                  count = length(Time))


    ##head(data)
    data$Method <- as.factor(data$Method)
    data$Method <- factor(data$Method, levels=ALGS)
    data <- data[order(data$Method),]

    shapes <- c()
    lines <- c()
    colors <- c()
    list[shapes, lines, colors] <- setup_shapes(data)

    data$Instance <- ""

    for(i in 1:nrow(data)){
        if(data[i,]$Size == 50000) data[i,]$Instance <- "Vl-1"
        else if(data[i,]$Size == 85900) data[i,]$Instance <- "Vl-2"
        else if(data[i,]$Size == 200000) data[i,]$Instance <- "Vl-3"
        else if(data[i,]$Size == 400000) data[i,]$Instance <- "Vl-4"
        else if(data[i,]$Size == 1000000) data[i,]$Instance <- "Vl-5"
    }
    ##print(data)

    plot <- ggplot(data, aes(x=Size, y=tavg, group=Method, color=Method, shape=Method, linetype=Method)) +
        labs(y = "Time (\u00B5s)", x = expression(paste("Instance"))) +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine")) +
        scale_shape_manual(values=shapes) +
        scale_linetype_manual(values=lines)+
        scale_color_manual(values=colors) +
        scale_y_continuous(trans='log10', label=scientific_10) +
        scale_x_continuous(trans='log10', labels=data$Instance, breaks=data$Size) +
        geom_point(size=2, alpha=0.6) +
        geom_line()

    ggsave(filename=gf1, width=6.5, height=4, plot=plot)
    embed_fonts(gf1)

}


create_cache_graphs <- function(){
    gf1 <- "../data/cache/graphs/graph_all_cache.pdf"
    data <- read.table("../data/cache/sum_cache.txt", header=T, sep=";")

    data <- data[data$Size > 30000,]
    data <- data[!data$Method == "ILP",]
    data$Instance <- sapply(X = data$Instance, FUN = toString)
    data$Instance <- sapply(X = data$Instance, FUN = firstup)

    datC <- data[(data[, "Metric"] == "Cache Hits" | data[, "Metric"] == "Cache Misses"),]
    datC$Type <- "Cache"
    datC$Metric <- factor(datC$Metric, levels = c("Cache Hits", "Cache Misses"))
    label_y <- expression(paste("Number of cache references"))

    cC <- c("#22a884", "#440154") ## special color set

    datC$Scale <- "Small scale"
    datC <- within(datC, Scale[Method == "CRA"] <- "Full scale")
    datCS <- datC
    datCS$Scale <- "Small scale"

    plot1 <- ggplot(datC, aes(fill=Metric, x=Method, y=Value)) +
        scale_fill_manual(values=cC) +
        ylab(label_y) +
        labs(x = "Method") +
        scale_y_continuous(label=scientific_10) +
        theme(text = element_text(size=10), axis.text.x = element_text(angle = 90, vjust = 0.5, hjust=1)) +
        geom_bar(data=transform(datC, Scale="Full scale"), position="stack", stat="identity") +
        geom_bar(data=datCS, position="stack", stat="identity") +
        theme(text = element_text(family = "Linux Libertine Capitals"), axis.title = element_text(family = "Linux Libertine"), legend.title = element_text(family = "Linux Libertine"), legend.text = element_text(family = "Linux Libertine"))

    ## now connect the 'Full scale' and 'Small scale' plots
    p1 <- plot1 + facet_grid(Scale ~ Instance, scales = "free") + theme(strip.text.y = element_text(family = "Linux Libertine"))
    g1 <- ggplotGrob(p1)

    #gtable_show_layout(g1)

    p2 <- plot1 + facet_grid(Scale ~ Instance, scales = "free") + coord_cartesian(ylim = c(0,1.5e9)) ##+ theme(strip.text.y = element_text(family = "Linux Libertine"))
    g2 <- ggplotGrob(p2)

    ## black magic to correctly place the graphs and legends
    ## this may stop working depending on ggplot's version (?)
    g1[["grobs"]][[3]] <- g2[["grobs"]][[2]]
    g1[["grobs"]][[5]] <- g2[["grobs"]][[4]]
    g1[["grobs"]][[7]] <- g2[["grobs"]][[6]]
    g1[["grobs"]][[9]] <- g2[["grobs"]][[8]]
    g1[["grobs"]][[11]] <- g2[["grobs"]][[10]]

    ## Tweak axis
    g1[["grobs"]][[23]] <- g2[["grobs"]][[22]]

    ggsave(filename=gf1, width=8, height=5, plot=g1)
    embed_fonts(gf1)
}





## Just call all functions!

create_sp_graphs()
create_nc_graphs()
create_vrp_graphs()
create_vl_graphs()
create_cache_graphs()
